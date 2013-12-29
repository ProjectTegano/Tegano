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
//
// Wolframe PostgreSQL client view implementation
//

#include "logger-v1.hpp"
#include "logger/logObject.hpp"
#include "PostgreSQL.hpp"
#include "utils/fileUtils.hpp"
#include "PostgreSQLtransactionExecStatemachine.hpp"
#include <string>
#include <sstream>

#include <boost/filesystem.hpp>

namespace _Wolframe {
namespace db {

//***  PostgreSQL database functions  ***************************************
static std::string escConnElement( std::string element )
{
	std::string esc;
	for ( std::string::const_iterator it = element.begin(); it != element.end(); it++ )	{
		if ( *it == '\'' )
			esc += "\\'";
		else if ( *it == '\\' )
			esc += "\\\\";
		else
			esc += *it;
	}
	return esc;
}

static std::string buildConnStr( const std::string& host, unsigned short port, const std::string& dbName,
				 const std::string& user, const std::string& password,
				 const std::string& sslMode, const std::string& sslCert, const std::string& sslKey,
				 const std::string& sslRootCert, const std::string& sslCRL,
				 unsigned short connectTimeout )
{
	std::stringstream ss;

	if ( ! host.empty())
		ss << "host = '" << host << "'";
	if ( port != 0 )	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "port = " << port;
	}
	if ( ! dbName.empty())	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "dbname = '" << escConnElement( dbName ) << "'";
	}
	if ( ! user.empty())	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "user = '" << escConnElement( user ) << "'";
		if ( ! password.empty())	{
			if ( ! ss.str().empty())
				ss << " ";
			ss << "password = '" << escConnElement( password ) << "'";
		}
	}
	if ( ! sslMode.empty())	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "sslmode = '" << escConnElement( sslMode ) << "'";
	}
	if ( ! sslCert.empty())	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "sslcert = '" << escConnElement( sslCert ) << "'";
		if ( ! sslKey.empty())	{
			if ( ! ss.str().empty())
				ss << " ";
			ss << "sslkey = '" << escConnElement( sslKey ) << "'";
		}
	}
	if ( ! sslRootCert.empty())	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "sslrootcert = '" << escConnElement( sslRootCert ) << "'";
	}
	if ( ! sslCRL.empty())	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "sslcrl = '" << escConnElement( sslCRL ) << "'";
	}
	if ( connectTimeout != 0 )	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "connect_timeout = " << connectTimeout;
	}
	return ss.str();
}

_Wolframe::log::LogLevel::Level PostgreSQLdbUnit::getLogLevel( const std::string& severity)
{
	struct LogMsgMap :public std::map<std::string,_Wolframe::log::LogLevel::Level>
	{
		LogMsgMap()
		{
			typedef _Wolframe::log::LogLevel LV;
			(*this)["WARNING"] = LV::LOGLEVEL_WARNING;
			(*this)["ERROR"] = LV::LOGLEVEL_ERROR;
			(*this)["FATAL"] = LV::LOGLEVEL_FATAL;
			(*this)["PANIC"] = LV::LOGLEVEL_ALERT;
			(*this)["NOTICE"] = LV::LOGLEVEL_NOTICE;
			(*this)["DETAIL"] = LV::LOGLEVEL_INFO;
			(*this)["INFO"] = LV::LOGLEVEL_INFO;
			(*this)["DEBUG"] = LV::LOGLEVEL_DEBUG;
			(*this)["LOG"] = LV::LOGLEVEL_TRACE;
		}
	};
	static LogMsgMap logMsgMap;
	LogMsgMap::const_iterator li = logMsgMap.find( severity);
	if (li == logMsgMap.end())
	{
		return log::LogLevel::LOGLEVEL_UNDEFINED;
	}
	else
	{
		return li->second;
	}
}

void PostgreSQLdbUnit::noticeProcessor( void* this_void, const char * message)
{
	PostgreSQLdbUnit* this_ = (PostgreSQLdbUnit*)this_void;
	std::size_t ii=0;
	for (; message[ii] && message[ii] != ':'; ++ii);
	if (message[ii])
	{
		_Wolframe::log::LogLevel::Level lv = getLogLevel( std::string( message, ii));
		if (lv != log::LogLevel::LOGLEVEL_UNDEFINED)
		{
			_Wolframe::log::Logger(logBackendPtr).Get( lv) << "PostgreSQL database '" << ((this_)?this_->ID():"") << "': " << (message + ii + 1);
			return;
		}
	}
	LOG_ERROR << "Unknown log message type from PostgreSQL database '" << ((this_)?this_->ID():"") << "': " << message;
}

// This function also needs a lot of work
PostgreSQLdbUnit::PostgreSQLdbUnit(const std::string& id,
				    const std::string& host, unsigned short port,
				    const std::string& dbName,
				    const std::string& user, const std::string& password,
				    std::string sslMode, std::string sslCert, std::string sslKey,
				    std::string sslRootCert, std::string sslCRL ,
				    unsigned short connectTimeout,
				    size_t connections, unsigned short acquireTimeout,
				    unsigned statementTimeout,
				    const std::list<std::string>& programFiles_)
	: m_ID( id ), m_noConnections( 0 ), m_connPool( acquireTimeout ),
	  m_statementTimeout( statementTimeout ), m_programFiles(programFiles_)
{
	m_connStr = buildConnStr( host, port,  dbName, user, password,
				  sslMode, sslCert, sslKey, sslRootCert, sslCRL,
				  connectTimeout );
	MOD_LOG_DATA << "PostgreSQL database '" << m_ID << "' connection string <" << m_connStr << ">";

	for ( size_t i = 0; i < connections; i++ )	{
		PGconn* conn = PQconnectdb( m_connStr.c_str() );
		if ( conn == NULL )
			MOD_LOG_ALERT << "Failed to connect to PostgreSQL database '" << m_ID << "'";
		else	{
			PQsetNoticeProcessor( conn, &noticeProcessor, this);

			ConnStatusType stat = PQstatus( conn );
			std::string err = PQerrorMessage( conn );
			while ( !err.empty() && ( err[ err.size() - 1 ] == '\r' ||  err[ err.size() - 1 ] == '\n' ))
				err.erase( err.size() - 1 );
			switch( stat )	{
				case CONNECTION_OK:
					MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connected OK.";
					break;
				case CONNECTION_BAD:
					if ( err.compare( 0, 6, "FATAL:" ) == 0 )	{
						MOD_LOG_ALERT << "PostgreSQL database '" << m_ID << "' (connection "
							      << i << ") error: '" << err << "'";
						throw std::runtime_error( "Fatal error connecting to the PostgreSQL database" );
					}
					MOD_LOG_WARNING << "PostgreSQL database '" << m_ID << "' connection "
							<< i << " error: '" << err << "'";
					break;
				case CONNECTION_STARTED:
					MOD_LOG_ALERT << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection STARTED: '" << err << "'";
					throw std::range_error( "PostgreSQL database: CONNECTION_STARTED in synchronous connection" );
					break;
				case CONNECTION_MADE:
					MOD_LOG_ALERT << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection MADE: '" << err << "'";
					throw std::range_error( "PostgreSQL database: CONNECTION_MADE in synchronous connection" );
					break;
				case CONNECTION_AWAITING_RESPONSE:
					MOD_LOG_ALERT << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection AWAITING RESPONSE: '" << err << "'";
					throw std::range_error( "PostgreSQL database: CONNECTION_AWAITING_RESPONSE in synchronous connection" );
					break;
				case CONNECTION_AUTH_OK:
					MOD_LOG_ALERT << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection AUTH OK: '" << err << "'";
					throw std::range_error( "PostgreSQL database: CONNECTION_AUTH_OK in synchronous connection" );
					break;
				case CONNECTION_SSL_STARTUP:
					MOD_LOG_ALERT << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection SSL start: '" << err << "'";
					throw std::range_error( "PostgreSQL database: CONNECTION_SSL_STARTUP in synchronous connection" );
					break;
				case CONNECTION_SETENV:
					MOD_LOG_ALERT << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection SETENV: '" << err << "'";
					throw std::range_error( "PostgreSQL database: CONNECTION_SETENV in synchronous connection" );
					break;
				case CONNECTION_NEEDED:
					MOD_LOG_ALERT << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection NEEDED: '" << err << "'";
					throw std::range_error( "PostgreSQL database: CONNECTION_NEEDED in synchronous connection" );
					break;
			}
			m_connPool.add( conn );
			m_noConnections++;
		}
	}

	m_db.setUnit( this );

	MOD_LOG_DEBUG << "PostgreSQL database '" << m_ID << "' created with a pool of " << m_noConnections << " connections";
}


// This function needs a lot of work and thinking...
PostgreSQLdbUnit::~PostgreSQLdbUnit()
{
	size_t connections = 0;

	m_db.setUnit( NULL );
	m_connPool.timeout( 3 );

	while ( m_connPool.available() )	{
		PGconn* conn = m_connPool.get();
		if ( conn == NULL )	{
			MOD_LOG_ALERT << "PostgreSQL database '" << m_ID << "' destructor: NULL connection from pool";
			throw std::logic_error( "PostgreSQL database destructor: NULL connection from pool" );
		}
		PGTransactionStatusType stat = PQtransactionStatus( conn );
		switch( stat )	{
			case PQTRANS_IDLE:
				PQfinish( conn );
				MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " idle";
				m_noConnections--, connections++;
				break;
			case PQTRANS_ACTIVE:
				PQfinish( conn );
				MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " active";
				m_noConnections--, connections++;
				break;
			case PQTRANS_INTRANS:
				PQfinish( conn );
				MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " in transaction";
				m_noConnections--, connections++;
				break;
			case PQTRANS_INERROR:
				PQfinish( conn );
				MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " in transaction error";
				m_noConnections--, connections++;
				break;
			case PQTRANS_UNKNOWN:
				PQfinish( conn );
				MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " status unknown";
				m_noConnections--, connections++;
				break;
		}
	}
	if ( m_noConnections != 0 )	{
		MOD_LOG_ALERT << "PostgreSQL database unit '" << m_ID << "' destructor: "
			      << m_noConnections << " connections not destroyed";
		throw std::logic_error( "PostgreSQL database unit destructor: not all connections destroyed" );
	}
	MOD_LOG_TRACE << "PostgreSQL database unit '" << m_ID << "' destroyed, " << connections << " connections destroyed";
}

void PostgreSQLdbUnit::loadProgram( const std::string& filename )
{
	// No program file, do nothing
	if ( filename.empty())
		return;
	if ( !boost::filesystem::exists( filename ))	{
		MOD_LOG_ALERT << "Program file '" << filename
			      << "' does not exist (PostgreSQL database '" << m_ID << "')";
		return;
	}
	try
	{
		addProgram( utils::readSourceFileContent( filename));
	}
	catch (const std::runtime_error& e)
	{
		throw std::runtime_error( std::string("error in program '") + utils::getFileStem(filename) + "':" + e.what());
	}
}

void PostgreSQLdbUnit::loadAllPrograms()
{
	std::list<std::string>::const_iterator pi = m_programFiles.begin(), pe = m_programFiles.end();
	for (; pi != pe; ++pi)
	{
		MOD_LOG_DEBUG << "Load Program '" << *pi << "' for PostgreSQL database unit '" << m_ID << "'";
		loadProgram( *pi);
	}
	MOD_LOG_DEBUG << "Programs for PostgreSQL database unit '" << m_ID << "' loaded";
}


Database* PostgreSQLdbUnit::database()
{
	return m_db.hasUnit() ? &m_db : NULL;
}



/*****  PostgreSQL database  ******************************************/
const std::string& PostgreSQLdatabase::ID() const
{
	if ( m_unit )
		return m_unit->ID();
	else
		throw std::runtime_error( "PostgreSQL database unit not initialized" );
}

void PostgreSQLdatabase::loadProgram( const std::string& filename )
{
	if ( !m_unit )
		throw std::runtime_error( "loadProgram: PostgreSQL database unit not initialized" );
	m_unit->loadProgram( filename );
}

void PostgreSQLdatabase::loadAllPrograms()
{
	if ( !m_unit )
		throw std::runtime_error( "loadAllPrograms: PostgreSQL database unit not initialized" );
	m_unit->loadAllPrograms();
}

void PostgreSQLdatabase::addProgram( const std::string& program )
{
	if ( !m_unit )
		throw std::runtime_error( "addProgram: PostgreSQL database unit not initialized" );
	m_unit->addProgram( program );
}

Transaction* PostgreSQLdatabase::transaction( const std::string& name)
{
	return new PostgreSQLtransaction( *this, name);
}

void PostgreSQLdatabase::closeTransaction( Transaction *t )
{
	delete t;
}


PostgreSQLtransaction::PostgreSQLtransaction( PostgreSQLdatabase& database, const std::string& name_)
	:StatemachineBasedTransaction( name_, new TransactionExecStatemachine_postgres( name_, &database.dbUnit())){}

}} // _Wolframe::db

