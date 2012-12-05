/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
// Wolframe SQLite client view implementation
//

#include "logger-v1.hpp"
#include "utils/miscUtils.hpp"
#include "SQLite.hpp"
#include "SQLitePreparedStatement.hpp"
#include <boost/filesystem.hpp>
#include "sqlite3.h"
#undef LOWLEVEL_DEBUG

namespace _Wolframe {
namespace db {

extern "C" void profiling_callback(  void */*a*/, const char *b, sqlite3_uint64 c )
{
	LOG_DATA << b << " (time: " << c / 1000 << " ms)";
}

SQLiteDBunit::SQLiteDBunit( const std::string& id, const std::string& filename,
			    bool foreignKeys, bool profiling,
			    unsigned short connections,
			    const std::list<std::string>& programFiles_)
	: m_ID( id ), m_filename( filename ), m_programFiles(programFiles_)
{
	bool	checked = false;
	int	dbFlags = SQLITE_OPEN_READWRITE;

	if ( ! sqlite3_threadsafe() )	{
		if ( connections != 1 )	{
			LOG_WARNING << "SQLite database '" << id
				    << "' has not been compiled without the SQLITE_THREADSAFE parameter."
				    << " Using only 1 connection instead of " << connections << ".";
			connections = 1;
		}
		else	{
			if ( sqlite3_config( SQLITE_CONFIG_MULTITHREAD ) != SQLITE_OK )	{
				throw std::runtime_error( "Unable to set SQLite in multithreaded mode" );
			}
			dbFlags |= SQLITE_OPEN_NOMUTEX;
//			dbFlags |= SQLITE_OPEN_FULLMUTEX;
		}
	}

	for( int i = 0; i < connections; i++ ) {
		sqlite3 *handle;
		char* err;
		int res = sqlite3_open_v2( m_filename.c_str( ), &handle, dbFlags,
#ifndef _WIN32
					   "unix" );
#else
					   NULL );
#endif
		if( res != SQLITE_OK )	{
			MOD_LOG_ALERT << "Unable to open SQLite database '" << filename
				      << "': " << sqlite3_errmsg( handle );
				sqlite3_close( handle );	// really ?!?
			throw std::runtime_error( "Unable to open SQLite database" );
		}
		else	{
			if ( !checked )	{
				res = sqlite3_exec( handle, "PRAGMA integrity_check", NULL, NULL, &err );
				if( res != SQLITE_OK )	{
					MOD_LOG_ALERT << "Corrupt SQLite database '" << filename
						      << "': " << err;
					sqlite3_close( handle );
				}
				if( err )
					sqlite3_free( err );
				if( res != SQLITE_OK )
					throw std::runtime_error( "Corrupt SQLite database" );
				checked = true;
			}

			// enable foreign keys
			if ( foreignKeys )	{
				res = sqlite3_exec( handle, "PRAGMA foreign_keys=true", NULL, NULL, &err );
				if( res != SQLITE_OK ) {
					MOD_LOG_ALERT << "Unable to enforce integrity checks in '" << filename
						      << "': " << err;
				}
				if( err ) {
					sqlite3_free( err );
				}
			}
			// enable tracing and profiling of commands
			if ( profiling )
				sqlite3_profile( handle, profiling_callback, NULL );

			m_connections.push_back( handle );
			m_connPool.add( handle );
		}
	}
	m_db.setUnit( this );

	MOD_LOG_DEBUG << "SQLite database unit '" << m_ID << "' created with "
		      << connections << " connections to file '" << m_filename << "'";
}

SQLiteDBunit::~SQLiteDBunit( )
{
	m_db.setUnit( NULL );

	while( m_connPool.available( ) > 0 ) {
		sqlite3 *handle = m_connPool.get( );
		sqlite3_close( handle );
	}
	MOD_LOG_TRACE << "SQLite database unit '" << m_ID << "' destroyed";
}

void SQLiteDBunit::loadProgram( const std::string& filename )
{
	// No program file, do nothing
	if ( filename.empty())
		return;
	if ( !boost::filesystem::exists( filename ))	{
		MOD_LOG_ALERT << "Program file '" << filename
			      << "' does not exist (SQLite database '" << m_ID << "')";
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

void SQLiteDBunit::loadAllPrograms()
{
	std::list<std::string>::const_iterator pi = m_programFiles.begin(), pe = m_programFiles.end();
	for (; pi != pe; ++pi)
	{
		MOD_LOG_DEBUG << "Load Program '" << *pi << "' for SQLite database unit '" << m_ID << "'";
		loadProgram( *pi);
	}
	MOD_LOG_DEBUG << "Programs for SQLite database unit '" << m_ID << "' loaded";
}

Database* SQLiteDBunit::database()
{
	return m_db.hasUnit() ? &m_db : NULL;
}


/*****  SQLite database  **********************************************/
const std::string& SQLiteDatabase::ID() const
{
	if ( m_unit )
		return m_unit->ID();
	else
		throw std::runtime_error( "SQLite database unit not initialized" );
}

void SQLiteDatabase::loadProgram( const std::string& filename )
{
	if ( !m_unit )
		throw std::runtime_error( "loadProgram: SQLite database unit not initialized" );
	m_unit->loadProgram( filename );
}

void SQLiteDatabase::loadAllPrograms()
{
	if ( !m_unit )
		throw std::runtime_error( "loadAllPrograms: SQLite database unit not initialized" );
	m_unit->loadAllPrograms();
}

void SQLiteDatabase::addProgram( const std::string& program )
{
	if ( !m_unit )
		throw std::runtime_error( "addProgram: SQLite database unit not initialized" );
	m_unit->addProgram( program );
}

void SQLiteDatabase::addStatements( const types::keymap<std::string>& stmmap_)
{
	if ( !m_unit )
		throw std::runtime_error( "addStatements: SQLite database unit not initialized" );
	m_unit->addStatements( stmmap_);
}

const std::string* SQLiteDatabase::getProgram( const std::string& name) const
{
	if ( !m_unit )
		throw std::runtime_error( "getProgram: SQLite database unit not initialized" );
	return m_unit->getProgram( name );
}

Transaction* SQLiteDatabase::transaction( const std::string& /*name*/ )
{
	return new SQLiteTransaction( *this );
}

void SQLiteDatabase::closeTransaction( Transaction *t )
{
	delete t;
}


const UI::UserInterfaceLibrary* SQLiteDatabase::UIlibrary() const
{
	return new SQLiteUIlibrary( *this );
}

/*****  SQLite transaction  *******************************************/
SQLiteTransaction::SQLiteTransaction( SQLiteDatabase& database )
	: m_db( database ), m_unit( database.dbUnit() )
{}

const std::string& SQLiteTransaction::databaseID() const
{
	return m_unit.ID();
}

void SQLiteTransaction::execute_statement( const char* stmstr )
{
#ifdef LOWLEVEL_DEBUG
	std::cerr << "CALL " << stmstr << std::endl;
#endif
	if ( !m_conn.get() )
		throw std::runtime_error( "executing transaction statement without transaction context" );
	bool success = true;
	std::ostringstream msg;
	sqlite3_stmt* inst = 0;
	int rc = sqlite3_prepare_v2( **m_conn, stmstr, -1, &inst, 0);

	if (rc != SQLITE_OK && rc != SQLITE_DONE)
	{
		const char* str = sqlite3_errmsg( **m_conn);
		int errcode = sqlite3_errcode( **m_conn);
		msg << "SQLite error " << errcode;
		if (errcode != 0) msg << " (error code " << errcode << ")";
		msg << "; message: '" << str << "'";
		success = false;
	}
	sqlite3_finalize( inst);
	if (!success) throw std::runtime_error( msg.str());
}

void SQLiteTransaction::begin()
{
	m_conn.reset( new PoolObject<sqlite3*>( m_unit.m_connPool));
	execute_statement( "BEGIN TRANSACTION;");
}

void SQLiteTransaction::commit()
{
	execute_statement( "COMMIT TRANSACTION;");
	m_conn.reset();
}

void SQLiteTransaction::rollback()
{
	execute_statement( "ROLLBACK TRANSACTION;");
	m_conn.reset();
}

void SQLiteTransaction::execute_with_autocommit()
{
	try
	{
		PoolObject<sqlite3*> conn( m_unit.m_connPool );
		PreparedStatementHandler_sqlite3 ph( *conn, m_unit.stmmap() );
		try
		{
			if (!ph.begin()
			||  !ph.doTransaction( m_input, m_output)
			||  !ph.commit())
			{
				const char* err = ph.getLastError();
				throw std::runtime_error(err?err:"unknown error");
			}
		}
		catch (const std::runtime_error& e)
		{
			MOD_LOG_ERROR << "error in sqlite database transaction: " << e.what();
			ph.rollback();
			throw std::runtime_error( std::string("transaction failed: ") + e.what());
		}
	}
	catch ( _Wolframe::ObjectPoolTimeout )
	{
		throw std::runtime_error("timeout in database connection pool object allocation");
	}
}

void SQLiteTransaction::execute_transaction_operation()
{
	PreparedStatementHandler_sqlite3 ph( **m_conn.get(), m_unit.stmmap(), true );
	try
	{
		if (!ph.doTransaction( m_input, m_output))
		{
			const char* err = ph.getLastError();
			throw std::runtime_error(err?err:"unknown error");
		}
	}
	catch (const std::runtime_error& e)
	{
		MOD_LOG_ERROR << "error in sqlite database transaction operation: " << e.what();
		ph.rollback();
		m_conn.reset();
		throw std::runtime_error( std::string("transaction operation failed: ") + e.what());
	}
}

void SQLiteTransaction::execute()
{
	if ( m_conn.get() )
	{
		execute_transaction_operation();
	}
	else
	{
		execute_with_autocommit();
	}
}

void SQLiteTransaction::close()
{
	if ( m_conn.get() )
		MOD_LOG_ERROR << "closed transaction without 'begin' or rollback";
	m_conn.reset();
	m_db.closeTransaction( this );
}


/*****  SQLite user interface library  ********************************/
SQLiteUIlibrary::SQLiteUIlibrary( const SQLiteDatabase &database )
	: m_unit( database.dbUnit() )
{}


const std::list< UI::InterfaceObject::Info > SQLiteUIlibrary::interface( const std::string& platform,
									 const std::string& role,
									 const std::string& culture,
									 const std::string& tag ) const
{
	std::list< std::string > roles;
	roles.push_back( role );
	return interface( platform, roles, culture, tag );
}

const std::list< UI::InterfaceObject::Info > SQLiteUIlibrary::interface( const std::string& platform,
									 const std::list< std::string >& roles,
									 const std::string& culture,
									 const std::string& /*tag*/ ) const
{
	std::list< UI::InterfaceObject::Info >	objs;

	bool condition = false;
	PoolObject< sqlite3* > conn( m_unit.m_connPool );
	sqlite3_stmt* ppStmt = NULL;
	std::ostringstream errMsg;
	bool success = true;

	std::string query = "SELECT platform.name, locale, typeName "
			"FROM UIobject JOIN Platform ON Platform.ID = UIobject.platformID "
			"JOIN UIobjectType ON UIobject.typeID=UIobjectType.ID";

	std::string cond = " WHERE ";
	if ( ! platform.empty() )	{
		cond += "upper( Platform.name ) = "
				+ boost::algorithm::to_upper_copy( platform );
		condition = true;
	}

	if ( ! roles.empty() )	{
		if ( condition )
			cond += "AND upper( Platform.name ) = "
					+ boost::algorithm::to_upper_copy( platform );
		else	{
			cond += "";
			condition = true;
		}
	}

	if ( ! culture.empty() )	{
		if ( condition )
			cond += "AND upper( culture ) = "
					+ boost::algorithm::to_upper_copy( culture );
		else	{
			cond += "upper( culture ) = "
					+ boost::algorithm::to_upper_copy( culture );
			condition = true;
		}
	}

	if ( condition )
		query += cond;
	int rc = sqlite3_prepare_v2( *conn, query.c_str(), -1, &ppStmt, NULL );
	if ( rc != SQLITE_OK )	{
		const char* str = sqlite3_errmsg( *conn );
		int errcode = sqlite3_errcode( *conn );
		errMsg << "SQLite error " << errcode << ": " << str;
		success = false;
	}
	else	{
		assert ( ppStmt != NULL );
		while ( success )	{
			rc = sqlite3_step( ppStmt );
			if ( rc == SQLITE_ROW )	{
				const unsigned char* text;
				text = sqlite3_column_text ( ppStmt, 0 );
				std::cout << text << ", ";
//				UI::InterfaceObject::Info info();
//				objs.push_back( info );
			}
			else if ( rc == SQLITE_DONE )
				break;
			else	{
				const char* str = sqlite3_errmsg( *conn );
				int errcode = sqlite3_errcode( *conn );
				errMsg << "SQLite error " << errcode << ": " << str;
				success = false;
			}
		}
	}
	sqlite3_finalize( ppStmt );
	if ( !success )
		throw std::runtime_error( errMsg.str() );

	return objs;
}

const UI::InterfaceObject SQLiteUIlibrary::object( const UI::InterfaceObject::Info& /*info*/ ) const
{
	UI::InterfaceObject	obj( "FORM", "Linux", "dummy test", "mo_MO", 01000000,
				     "Dummy form for now", "" );
	return obj;
}

void SQLiteUIlibrary::addObject( const UI::InterfaceObject& /*newObject*/ ) const
{
}

bool SQLiteUIlibrary::deleteObject( const UI::InterfaceObject::Info& /*info*/ ) const
{
	return true;
}

}} // _Wolframe::db
