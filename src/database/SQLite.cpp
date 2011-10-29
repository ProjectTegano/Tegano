/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
#include "SQLite.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "miscUtils.hpp"

#include "sqlite3.h"

namespace _Wolframe {
namespace db {

//***  SQLite configuration functions  **************************************
SQLiteConfig::SQLiteConfig( const char* name, const char* logParent, const char* logName )
	: config::ObjectConfiguration( name, logParent, logName )
{
	flag = false;
}

void SQLiteConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );

	os << indStr << sectionName() << ":" << std::endl;
	if ( ! m_ID.empty() )
		os << indStr << "   ID: " << m_ID << std::endl;
	os << indStr << "   Filename: " << filename << std::endl;
	os << indStr << "   Flags: " << (flag ? "True Flag" : "False Flag") << std::endl;
}

bool SQLiteConfig::check() const
{
	if ( filename.empty() )	{
		LOG_ERROR << logPrefix() << "SQLite database filename cannot be empty";
		return false;
	}
	return true;
}

void SQLiteConfig::setCanonicalPathes( const std::string& refPath )
{
	using namespace boost::filesystem;

	if ( ! filename.empty() )	{
		if ( ! path( filename ).is_absolute() )
			filename = resolvePath( absolute( filename,
							  path( refPath ).branch_path()).string());
		else
			filename = resolvePath( filename );
	}
}


//***  SQLite database functions  *******************************************
SQLiteDatabase::SQLiteDatabase( const std::string& id,
				const std::string& filename, unsigned short connections, bool flag )
	: m_ID( id ), m_filename( filename ), m_flag( flag )
{
	for( int i = 0; i < connections; i++ ) {
		sqlite3 *handle;
		int res = sqlite3_open( m_filename.c_str( ), &handle );
		if( res != SQLITE_OK ) {
			LOG_ERROR << "Unable to open Sqlite3 database '" << filename << ": " << sqlite3_errmsg( handle );
		}

		m_connections.push_back( handle );
		m_connPool.add( handle );
	}

	LOG_NOTICE << "SQLite database '" << m_ID << "' created with "
		   << "filename '" << m_filename << "'";
}

SQLiteDatabase::~SQLiteDatabase( )
{
	while( m_connPool.available( ) > 0 ) {
		sqlite3 *handle = m_connPool.get( );
		sqlite3_close( handle );
	}

	LOG_DEBUG << "SQLite database '" << m_ID << "' destroyed with "
		  << "filename '" << m_filename << "'";
}

bool SQLiteDatabase::doTransaction( DatabaseRequest& /*request*/, DatabaseAnswer& /*answer*/,
				    unsigned short /*timeout*/, unsigned short /*retries*/ )
{
	/*
// check if user is in the sqlite table
#if SQLITE_VERSION_NUMBER >= 3005000
   rc = sqlite3_prepare_v2( m_db, sql.c_str( ), -1, &stmt, &tail );
#else
   rc = sqlite3_prepare( m_db, sql.c_str( ), -1, &stmt, &tail );
#endif
   if( rc != SQLITE_OK ) {
    std::ostringstream oss;
    oss << "Unable to prepare SQL statement '" << sql << ": " << sqlite3_errmsg( m_db );
    throw std::runtime_error( oss.str( ) );
   }

   rc = sqlite3_bind_text( stmt, 1, m_login.c_str( ), m_login.length( ), SQLITE_STATIC );
   if( rc != SQLITE_OK ) {
    std::ostringstream oss;
    oss << "Unable to bind parameter login in '" << sql << ": " << sqlite3_errmsg( m_db );
    throw std::runtime_error( oss.str( ) );
   }

   rc = sqlite3_step( stmt );
   if( rc == SQLITE_DONE ) {
    m_state = _Wolframe_DB_SQLITE3_STATE_NEED_LOGIN;
    goto FAIL;
   } else if( rc == SQLITE_ROW ) {
    pass = (const char *)sqlite3_column_text( stmt, 0 );
   }

// user found, but password doesn't match
   if( strcmp( pass, m_pass.c_str( ) ) != 0 ) {
    m_state = _Wolframe_DB_SQLITE3_STATE_NEED_LOGIN;
    goto FAIL;
   }

   sqlite3_finalize( stmt );
*/
	return true;
}

//***  SQLite database unit  ************************************************
SQLiteUnit::SQLiteUnit( const SQLiteConfig& conf )
	: m_db( conf.m_ID, conf.filename, /* Aba: temporary */ 10, conf.flag )
{
	LOG_NOTICE << "SQLite database container for '" << conf.m_ID << "' created";
}

SQLiteUnit::~SQLiteUnit( )
{
}

}} // _Wolframe::db


