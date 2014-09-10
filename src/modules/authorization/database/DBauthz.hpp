/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
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
// database authorization
//

#ifndef _DB_AUTHORIZATION_HPP_INCLUDED
#define _DB_AUTHORIZATION_HPP_INCLUDED

#include "config/configurationObject.hpp"
#include "aaaa/authorizationUnit.hpp"
#include "module/configuredObjectConstructor.hpp"

namespace _Wolframe {
namespace aaaa {

//***  Database authorizer configuration  *******************************
class DatabaseAuthzConfig : public config::ConfigurationObject
{
public:
	DatabaseAuthzConfig( const std::string& className_, const std::string& configSection_, const std::string& configKeyword_)
		:config::ConfigurationObject( className_, configSection_, configKeyword_){}

	/// methods
	bool parse( const config::ConfigurationNode& pt, const std::string& node,
		    const module::ModuleDirectory* modules );
	bool check() const;

	void print( std::ostream& os, size_t indent ) const;

	const std::string& identifier() const	{return m_identifier;}
	const std::string& dbConfig() const	{return m_dbConfig;}

private:
	std::string	m_identifier;
	std::string	m_dbConfig;
};


//***  Database authorizer  *********************************************
class DBauthorizer : public AuthorizationUnit
{
public:
	DBauthorizer( const DatabaseAuthzConfig* config);
	~DBauthorizer();

	bool resolveDB( const db::DatabaseProviderInterface& db );

	AuthorizationUnit::Result allowed( const Information& );

private:
	const std::string	m_dbLabel;
	const db::Database*	m_db;

	AuthorizationUnit::Result connectionAllowed( const net::LocalEndpoint& /*local*/,
						     const net::RemoteEndpoint& remote );
};

}}
#endif
