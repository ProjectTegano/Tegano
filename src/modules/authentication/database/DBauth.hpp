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
// database authenticator
//

#ifndef _DB_AUTHENTICATION_HPP_INCLUDED
#define _DB_AUTHENTICATION_HPP_INCLUDED

#include "aaaa/authenticationUnit.hpp"
#include "module/configuredObjectConstructor.hpp"
#include "config/configurationObject.hpp"
#include "config/configurationTree.hpp"

namespace _Wolframe {
namespace aaaa {

static const char* DB_AUTHENTICATION_CLASS_NAME = "DBAuth";

class DBAuthConfig : public config::ConfigurationObject
{
public:
	DBAuthConfig( const std::string& className_, const std::string& configSection_, const std::string& configKeyword_)
		:config::ConfigurationObject( className_, configSection_, configKeyword_){}

	bool parse( const config::ConfigurationNode& pt, const std::string& node,
		    const module::ModuleDirectory* modules );
	bool check() const;

	void print( std::ostream& os, size_t indent ) const;

	void setCanonicalPathes( const std::string& /*refPath*/ ){}

	const std::string& identifier() const	{return m_identifier;}
	const std::string& dbLabel() const	{return m_dbLabel;}

private:
	std::string	m_identifier;
	std::string	m_dbLabel;
};


class DBauthUnit : public AuthenticationUnit
{
public:
	DBauthUnit( const DBAuthConfig* config);
	~DBauthUnit();

	const char* className() const			{ return DB_AUTHENTICATION_CLASS_NAME; }

	bool resolveDB( const db::DatabaseProviderInterface& db );

	const char** mechs() const;

	AuthenticatorSlice* slice( const std::string& mech,
				   const net::RemoteEndpoint& client );

private:
	const std::string		m_dbLabel;
	const db::Database*		m_db;
};

}}
#endif
