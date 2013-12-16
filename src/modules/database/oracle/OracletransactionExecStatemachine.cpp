/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\brief Implementation of the standard database transaction execution statemechine with Oracle
///\file OracletransactionExecStatemachine.cpp

#include "OracletransactionExecStatemachine.hpp"
#include "Oracle.hpp"
#include "logger-v1.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

TransactionExecStatemachine_oracle::TransactionExecStatemachine_oracle( OracleConnection* conn_, const std::string& dbname_, bool inTransactionContext)
	:m_state(inTransactionContext?Transaction:Init)
	,m_conn(conn_)
	,m_dbname(dbname_)
	,m_lastresult(0)
	,m_nof_rows(0)
	,m_idx_row(0)
	,m_hasResult(false){}

TransactionExecStatemachine_oracle::~TransactionExecStatemachine_oracle()
{
	clear();
}

void TransactionExecStatemachine_oracle::clear()
{
	if (m_lastresult)
	{
//		PQclear( m_lastresult);
		m_lastresult = 0;
	}
	m_lasterror.reset();
	m_statement.clear();
	m_state = Init;
	m_nof_rows = 0;
	m_idx_row = 0;
	m_hasResult = false;
}

static const char* getErrorType( const char* /* tp */)
{
	return 0;
#if 0
	if (!tp) return 0;
	if (strcmp (tp,"01007") == 0 || strcmp (tp,"01008") == 0)
	{
		return "PRIVILEGE";
	}
	if (memcmp( tp, "0L", 2) == 0 || memcmp( tp, "0P", 2) == 0 || memcmp( tp, "28", 2) == 0)
	{
		return "PRIVILEGE";
	}
	if (memcmp( tp, "03", 2) == 0)
	{
		return "STATEMENT";
	}
	if (memcmp( tp, "08", 2) == 0)
	{
		return "CONNECTION";
	}
	if (memcmp( tp, "09", 2) == 0 || memcmp( tp, "2F", 2) == 0 || memcmp( tp, "38", 2) == 0 || memcmp( tp, "39", 2) == 0 || memcmp( tp, "3B", 2) == 0)
	{
		return "EXCEPTION";
	}
	if (memcmp( tp, "21", 2) == 0)
	{
		return "PARAMETER";
	}
	if (memcmp( tp, "22", 2) == 0)
	{
		return "PARAMETER";
	}
	if (memcmp( tp, "23", 2) == 0)
	{
		return "CONSTRAINT";
	}
	if (memcmp( tp, "42", 2) == 0)
	{
		return "SYNTAX";
	}
	if (memcmp( tp, "53", 2) == 0)
	{
		return "RESOURCE";
	}
	if (memcmp( tp, "5", 1) == 0)
	{
		return "SYSTEM";
	}
	if (memcmp( tp, "F0", 2) == 0)
	{
		return "CONFIGURATION";
	}
	if (memcmp( tp, "P0", 2) == 0)
	{
		return "PLSQL";
	}
	if (memcmp( tp, "XX", 2) == 0)
	{
		return "INTERNAL";
	}
	return 0;
#endif
}

void TransactionExecStatemachine_oracle::setDatabaseErrorMessage( sword status )
{
	sb4 errcode = 0;
	char errmsg[512];

	switch( status ) {
		case OCI_SUCCESS:
			strcpy( errmsg, "OCI_SUCCESS" );
			break;

		case OCI_SUCCESS_WITH_INFO:
			strcpy( errmsg, "OCI_SUCCESS_WITH_INFO" );
			break;
		
		case OCI_NEED_DATA:
			strcpy( errmsg, "OCI_NEED_DATA" );
			break;
		
		case OCI_NO_DATA:
			strcpy( errmsg, "OCI_NO_DATA" );
			break;
		
		case OCI_INVALID_HANDLE:
			strcpy( errmsg, "OCI_INVALID_HANDLE" );
			break;
		
		case OCI_STILL_EXECUTING:
			strcpy( errmsg, "OCI_STILL_EXECUTING" );
			break;
		
		case OCI_CONTINUE:
			strcpy( errmsg, "OCI_CONTINUE" );
			break;
			
		case OCI_ERROR:
			(void)OCIErrorGet( (dvoid *)m_conn->errhp, (ub4)1, (text *)NULL,
				&errcode, (text *)errmsg, (ub4)sizeof( errmsg ), OCI_HTYPE_ERROR );
			break;
	}
	
	//~ const char* errtype = m_lastresult?getErrorType( PQresultErrorField( m_lastresult, PG_DIAG_SQLSTATE)):"INTERNAL";
	//~ const char* severitystr = m_lastresult?PQresultErrorField( m_lastresult, PG_DIAG_SEVERITY):"ERROR";
	//~ log::LogLevel::Level severity = OracledbUnit::getLogLevel( severitystr);
	const char *usermsg = 0;
	const char *errtype = "INTERNAL";
	log::LogLevel::Level severity = log::LogLevel::LOGLEVEL_ERROR;
	if (errmsg)
	{
		usermsg = strstr( errmsg, "DETAIL:");
		if (usermsg) usermsg = usermsg + 7;
		if (!usermsg)
		{
			usermsg = strstr( errmsg, "ERROR:");
			if (usermsg) usermsg = usermsg + 7;
		}
		if (!usermsg)
		{
			usermsg = errmsg;
		}
	}
	m_lasterror.reset( new DatabaseError( severity, errcode, m_dbname.c_str(), m_statement.string().c_str(), errtype, errmsg, usermsg));
}

// was PQResult
// bool TransactionExecStatemachine_oracle::status( OracleStatement* res, State newstate)
bool TransactionExecStatemachine_oracle::status( sword status, State newstate )
{
	bool rt;

	// TODO: check better for status
	
	if( status == OCI_SUCCESS ) {
		// TODO: m_hasResult = xx;
		m_hasResult = false;
		m_state = newstate;
		rt = true;
	} else {
		setDatabaseErrorMessage( status );
		m_state = Error;
		rt = false;
	}
	
	// TODO: close results sets, cleanup
		
	return rt;
}

bool TransactionExecStatemachine_oracle::begin()
{
	LOG_TRACE << "[oracle statement] CALL begin()";
	if (m_state != Init)
	{
		return errorStatus( std::string( "call of begin not allowed in state '") + stateName(m_state) + "'");
	}

	return status( OCITransStart( m_conn->svchp, m_conn->errhp, (uword)0, (ub4)OCI_TRANS_NEW ), Transaction );
}

bool TransactionExecStatemachine_oracle::commit()
{
	LOG_TRACE << "[oracle statement] CALL commit()";
	if (m_state == Transaction)
	{
		LOG_WARNING << "executed transaction is empty";
	}
	else if (m_state != Executed && m_state != CommandReady)
	{
		return errorStatus( std::string( "call of commit not allowed in state '") + stateName(m_state) + "'");
	}
	
	return status( OCITransCommit( m_conn->svchp, m_conn->errhp, OCI_DEFAULT ), Init );
}

bool TransactionExecStatemachine_oracle::rollback()
{
	LOG_TRACE << "[oracle statement] CALL rollback()";

	return status( OCITransRollback( m_conn->svchp, m_conn->errhp, OCI_DEFAULT ), Init );
}

bool TransactionExecStatemachine_oracle::errorStatus( const std::string& message)
{
	if (m_state != Error)
	{
		m_lasterror.reset( new DatabaseError( log::LogLevel::LOGLEVEL_ERROR, 0, m_dbname.c_str(), m_statement.string().c_str(), "INTERNAL", message.c_str(), "internal logic error"));
		m_state = Error;
	}
	return false;
}

bool TransactionExecStatemachine_oracle::start( const std::string& statement)
{
	LOG_TRACE << "[oracle statement] CALL start (" << statement << ")";
	if (m_state == Executed || m_state == CommandReady)
	{
		m_state = Transaction;
	}
	if (m_state != Transaction)
	{
		return errorStatus( std::string( "call of start not allowed in state '") + stateName(m_state) + "'");
	}
	clear();
	m_statement.init( statement);
	m_state = CommandReady;
	return true;
}

bool TransactionExecStatemachine_oracle::bind( std::size_t idx, const types::VariantConst& value)
{
	if (value.defined())
	{
		LOG_TRACE << "[oracle statement] CALL bind( " << idx << ", '" << value << "' )";
	}
	else
	{
		LOG_TRACE << "[oracle statement] CALL bind( " << idx << ", NULL)";
	}
	if (m_state != CommandReady && m_state != Executed)
	{
		return errorStatus( std::string( "call of bind not allowed in state '") + stateName(m_state) + "'");
	}
	if (idx == 0 || idx > m_statement.maxparam())
	{
		errorStatus( std::string( "index of parameter out of range (") + boost::lexical_cast<std::string>(idx) + " required to be in range 1.." + boost::lexical_cast<std::string>(m_statement.maxparam()) + " in statement '" + m_statement.string() + "'");
		return false;
	}
	if (idx == 0 || idx > m_statement.maxparam())
	{
		errorStatus( std::string( "index of bind parameter out of range (") + boost::lexical_cast<std::string>(idx) + " required to be in range 1.." + boost::lexical_cast<std::string>(m_statement.maxparam()) + " in statement '" + m_statement.string() + "'");
		return false;
	}
	if (value.defined())
	{
		std::string strval = value.tostring();
		char* encvalue = (char*)std::malloc( strval.size() * 2 + 3);
		encvalue[0] = '\'';
		boost::shared_ptr<void> encvaluer( encvalue, std::free);
		int error = 0;
//		size_t encvaluesize = PQescapeStringConn( m_conn, encvalue+1, strval.c_str(), strval.size(), &error);
		size_t encvaluesize = 0;
		encvalue[encvaluesize+1] = '\'';
		std::string bindval( encvalue, encvaluesize+2);
		m_statement.bind( idx, bindval);
	}
	else
	{
		m_statement.bind( idx, "NULL");
	}
	m_state = CommandReady;
	return true;
}

bool TransactionExecStatemachine_oracle::execute()
{
	if (m_state != CommandReady)
	{
		return errorStatus( std::string( "call of execute not allowed in state '") + stateName(m_state) + "'");
	}
	if (m_lastresult)
	{
//		PQclear( m_lastresult);
		m_lastresult = 0;
	}
	std::string stmstr = m_statement.expanded();
	LOG_TRACE << "[oracle statement] CALL execute(" << stmstr << ")";
	sword status;
	status = O
//	m_lastresult = PQexec( m_conn, stmstr.c_str());

/*
			status = OCIHandleAlloc( envhp, (dvoid **)&stmthp,
				OCI_HTYPE_STMT, (size_t)0, (dvoid **)0 );
			if( status != OCI_SUCCESS ) goto cleanup;
			
			status = OCIStmtPrepare( stmthp, errhp, 
				(text *)const_cast<char *>( dbcmd.c_str( ) ),
				(ub4)dbcmd.length( ), (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT );
			if( status != OCI_SUCCESS ) goto cleanup;

			status = OCIStmtExecute( svchp, stmthp, errhp, (ub4)1, (ub4)0,
				NULL, NULL, OCI_DEFAULT );
			if( status != OCI_SUCCESS && status != OCI_SUCCESS_WITH_INFO ) goto cleanup;

			if( stmthp ) (void)OCIHandleFree( stmthp, OCI_HTYPE_STMT );
 */
	bool rt = status( 0, Executed);
	if (rt)
	{
		if (m_hasResult)
		{
//			m_nof_rows = (std::size_t)PQntuples( m_lastresult);
			m_idx_row = 0;
		}
		else
		{
			m_nof_rows = 0;
			m_idx_row = 0;
		}
	}
	return rt;
}

bool TransactionExecStatemachine_oracle::hasResult()
{
	return m_hasResult && m_nof_rows > 0;
}

std::size_t TransactionExecStatemachine_oracle::nofColumns()
{
	if (m_state != Executed)
	{
		return errorStatus( std::string( "inspect command result not possible in state '") + stateName(m_state) + "'");
	}
	if (!m_lastresult)
	{
		return errorStatus( "command result is empty");
	}
//	return PQnfields( m_lastresult);
	return 0;
}

const char* TransactionExecStatemachine_oracle::columnName( std::size_t idx)
{
	if (m_state != Executed)
	{
		errorStatus( std::string( "inspect command result not possible in state '") + stateName(m_state) + "'");
		return 0;
	}
	if (!m_lastresult)
	{
		errorStatus( "command result is empty");
		return 0;
	}
//	const char* rt = PQfname( m_lastresult, (int)idx-1);
	const char* rt = "";
	if (!rt) errorStatus( std::string( "index of column out of range (") + boost::lexical_cast<std::string>(idx) + ")");
	return rt;
}

const DatabaseError* TransactionExecStatemachine_oracle::getLastError()
{
	return m_lasterror.get();
}

types::VariantConst TransactionExecStatemachine_oracle::get( std::size_t idx)
{
	if (m_state != Executed)
	{
		errorStatus( std::string( "inspect command result not possible in state '") + stateName(m_state) + "'");
		LOG_DATA << "[oracle statement] CALL get(" << idx << ") => NULL";
		return types::VariantConst();
	}
	if (!m_lastresult)
	{
		errorStatus( "command result is empty");
		LOG_DATA << "[oracle statement] CALL get(" << idx << ") => NULL";
		return types::VariantConst();
	}
	if (m_idx_row >= m_nof_rows) return types::VariantConst();
//	char* resval = PQgetvalue( m_lastresult, (int)m_idx_row, (int)idx-1);
	char *resval = "";
	if (!resval || resval[0] == '\0')
	{
//		if (PQgetisnull( m_lastresult, (int)m_idx_row, (int)idx-1))
		{
			return types::VariantConst();
		}
	}
	types::VariantConst rt( resval);
	LOG_DATA << "[oracle statement] CALL get(" << idx << ") => " << rt.typeName() << " '" << rt.tostring() << "'";
	return rt;
}

bool TransactionExecStatemachine_oracle::next()
{
	LOG_TRACE << "[oracle statement] CALL next()";
	if (m_state != Executed)
	{
		errorStatus( std::string( "get next command result not possible in state '") + stateName(m_state) + "'");
		return false;
	}
	if (m_idx_row < m_nof_rows)
	{
		++m_idx_row;
		return (m_idx_row < m_nof_rows);
	}
	return false;
}


