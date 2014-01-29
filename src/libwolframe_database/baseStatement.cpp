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
///\brief Implements substitution of parameters in embedded SQL statements
///\file baseStatement.cpp

#include "database/baseStatement.hpp"
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <algorithm>
#include <cctype>

using namespace _Wolframe;
using namespace _Wolframe::db;

BaseStatement::BaseStatement( )
	: m_maxParam( 0 )
{
}

BaseStatement::BaseStatement( const BaseStatement &o )
	: m_stmtStr( o.m_stmtStr ),
	m_maxParam( o.m_maxParam ),
	m_data( o.m_data ),
	m_nativeStmt( o.m_nativeStmt )
{
}

BaseStatement::BaseStatement( const std::string &stmtStr )
	: m_stmtStr( stmtStr ),
	m_maxParam( 0 )
{
	parse( );
}

void BaseStatement::init( const std::string &stmtStr )
{
	clear( );
	m_stmtStr = stmtStr;
	parse( );
}

void BaseStatement::clear( )
{
	m_stmtStr.clear( );
	m_maxParam = 0;
	m_data.clear();
	m_nativeStmt.clear( );
	m_usedIdx.reserve( 32 );
	std::fill_n( m_usedIdx.begin( ), 32, false );
}

void BaseStatement::bind( const unsigned int idx, const types::Variant & /*value*/ )
{
	if( idx < 1 || idx > m_maxParam ) {
		throw std::runtime_error(
			"index of bind parameter is out of range " +
			boost::lexical_cast<std::string>( idx ) +
			" required to be in range 1.." +
			boost::lexical_cast<std::string>( m_maxParam ) +
			" in statement '" + originalSQL( ) + "'"
		);
	}
	if( !m_usedIdx[idx] ) {
		throw std::runtime_error(
			"setting value for undefined index " +
			boost::lexical_cast<std::string>( idx ) +
			" in statement '" + originalSQL( ) + "'"			
		);
	}
}

const std::string BaseStatement::originalSQL( ) const
{
	return m_stmtStr;
}

const std::string BaseStatement::nativeSQL( ) const
{
	return m_nativeStmt;
}

void BaseStatement::parse( )
{	
	std::string::const_iterator si = m_stmtStr.begin( ), se = m_stmtStr.end( );
	std::string::const_iterator chunkstart = si;
	for( ; si != se; si++ ) {
		// don't search for placeholders in strings
		if( *si == '\'' || *si == '\"' ) {
			char eb = *si;
			si++;
			for( ; si != se && *si != eb; si++ ) {
				// allow escaping of the same quote
				if( *si == '\\' ) {
					si++;
					if( si == se ) break;
				}
			}
			if( si == se ) {
				throw std::runtime_error( "string not terminated in statement '" +
					originalSQL( ) + "'" );
			}
		} else if( *si == '$' ) {
			
			if( si > chunkstart ) {
				m_data.push_back( Element( 0, std::string( chunkstart, si ) ) );
				chunkstart = si;
			}
			
			si++;
			unsigned int idx = 0;				
			for( ; si != se && isdigit( *si ); si++ ) {
				idx *= 10;
				// legal to assume this, see 5.2.1 of C11..
				idx += ( *si - '0' );
			}
			if( idx == 0 ) {
				if( se == se ) {
					throw std::runtime_error( "illegal parameter index (lonely $ at the end) in statement'" +
						originalSQL( ) + "'" );
				} else if( isalpha( *si ) ) {
					throw std::runtime_error( "illegal parameter index (immediately followed by identifier) in statement '" +
						originalSQL( ) + "'" );
				} else if( *si == '_' ) {
					throw std::runtime_error( "illegal parameter index (immediately followed by underscore) in statement '" +
						originalSQL( ) + "'" );
				} else {
					throw std::runtime_error( "illegal parameter index seen statement '" +
					originalSQL( ) + "' at position " + boost::lexical_cast<std::string>( se -si ) );
				}
			}

			if( idx > m_maxParam ) {
				m_usedIdx.reserve( idx + 1 );
				std::fill_n( m_usedIdx.begin( ), idx+1, false );
			}
			m_usedIdx[idx] = true;
			m_maxParam = idx;

			chunkstart = si;
						
			m_data.push_back( Element( idx, "" ) );
		}
	}
	if( si > chunkstart ) {
		m_data.push_back( Element( 0, std::string( chunkstart, si) ) );
	}
}

void BaseStatement::substitute( )
{
	m_nativeStmt.clear( );
	std::vector<Element>::const_iterator di = m_data.begin( ), de = m_data.end( );
	for( ; di != de; di++ ) {
		if( di->first ) {
			// a placeholder
			m_nativeStmt.append( replace( di->first ) );
		} else {
			// a chunk of normal SQL statement
			m_nativeStmt.append( di->second );
		}
	}
}
