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
// Wolframe Postgresql client configuration
//

#ifndef _POSTGRESQL_CONFIG_HPP_INCLUDED
#define _POSTGRESQL_CONFIG_HPP_INCLUDED

#include "config/configurationObject.hpp"
#include "serialize/configSerialize.hpp"
#include "serialize/descriptiveConfiguration.hpp"
#include <vector>

namespace _Wolframe {
namespace db {

enum {
	DEFAULT_POSTGRESQL_PORT = 5432,
	DEFAULT_POSTGRESQL_CONNECTIONS = 4,
	DEFAULT_POSTGRESQL_CONNECTION_TIMEOUT = 30,
	DEFAULT_POSTGRESQL_STATEMENT_TIMEOUT = 30000,
	DEFAULT_POSTGRESQL_ACQUIRE_TIMEOUT = 0
};

/// \brief PostgreSQL database configuration
class PostgreSQLConfig
	:public _Wolframe::serialize::DescriptiveConfiguration
{
public:
	explicit PostgreSQLConfig( const std::string& className_="PostgreSQL", const std::string& configSection_="Database", const std::string& configKeyword_="PostgreSQL")
		:_Wolframe::serialize::DescriptiveConfiguration( className_, configSection_, configKeyword_, getStructDescription())
		,m_port(DEFAULT_POSTGRESQL_PORT)
		,m_connectTimeout(DEFAULT_POSTGRESQL_CONNECTION_TIMEOUT)
		,m_connections(DEFAULT_POSTGRESQL_CONNECTIONS)
		,m_acquireTimeout(DEFAULT_POSTGRESQL_ACQUIRE_TIMEOUT)
		,m_statementTimeout(DEFAULT_POSTGRESQL_STATEMENT_TIMEOUT)
	{
		setBasePtr( (void*)this); // ... mandatory to set pointer to start of configuration
	}

	PostgreSQLConfig( const std::string& id_, const std::string& host_,
			unsigned short port_, const std::string& dbName_,
			const std::string& user_, const std::string& password_,
			const std::string& sslMode_, const std::string& sslCert_,
			const std::string& sslKey_, const std::string& sslRootCert_,
			const std::string& sslCRL_,			
			unsigned short connectTimeout_, unsigned short connections_,
			unsigned short acquireTimeout_, unsigned statementTimeout_)
		:_Wolframe::serialize::DescriptiveConfiguration( "PostgreSQL" /*section name*/, "database" /*log parent*/, "PostgreSQL" /*log name*/, getStructDescription())
		,m_id(id_)
		,m_host(host_)
		,m_port(port_)
		,m_dbName(dbName_)
		,m_user(user_)
		,m_password(password_)
		,m_sslMode(sslMode_)
		,m_sslCert(sslCert_)
		,m_sslKey(sslKey_)
		,m_sslRootCert(sslRootCert_)
		,m_sslCRL(sslCRL_)
		,m_connectTimeout(connectTimeout_)
		,m_connections(connections_)
		,m_acquireTimeout(acquireTimeout_)
		,m_statementTimeout(statementTimeout_){}

	virtual bool parse( const config::ConfigurationNode& cfgTree, const std::string& node,
			    const module::ModuleDirectory* modules );
	virtual bool check() const;
	virtual void print( std::ostream& os, size_t indent ) const;
	virtual void setCanonicalPathes( const std::string& referencePath );

	const std::string& id() const			{ return m_id; }
	const std::string& host() const			{ return m_host; }
	unsigned short port() const			{ return m_port; }
	const std::string& dbName() const		{ return m_dbName; }
	const std::string& user() const			{ return m_user; }
	const std::string& password() const		{ return m_password; }
	const std::string& sslMode() const		{ return m_sslMode; }
	const std::string& sslCert() const		{ return m_sslCert; }
	const std::string& sslKey() const		{ return m_sslKey; }
	const std::string& sslRootCert() const		{ return m_sslRootCert; }
	const std::string& sslCRL() const		{ return m_sslCRL; }
	unsigned short connectTimeout() const		{ return m_connectTimeout; }
	unsigned short connections() const		{ return m_connections; }
	unsigned short acquireTimeout() const		{ return m_acquireTimeout; }
	unsigned statementTimeout() const		{ return m_statementTimeout; }

public:
	/// \brief Structure description for serialization/parsing
	static const serialize::StructDescriptionBase* getStructDescription();

private:
	std::string	m_id;			//< database identifier
	std::string	m_host;			//< server host
	unsigned short	m_port;			//< server port
	std::string	m_dbName;		//< database name on server
	std::string	m_user;			//< database user
	std::string	m_password;		//< and password
	std::string	m_sslMode;		//< SSL connection mode
	std::string	m_sslCert;		//< client SSL certificate file
	std::string	m_sslKey;		//< client SSL key file
	std::string	m_sslRootCert;		//< root SSL certificate file
	std::string	m_sslCRL;		//< SSL certificate revocation list
	unsigned short	m_connectTimeout;	//< connection timeout
	unsigned short	m_connections;	 	//< number of database connection (pool size)
	unsigned short	m_acquireTimeout;	//< timeout when acquiring a connection from the pool
	unsigned	m_statementTimeout;	//< default timeout when executin a statement

	config::ConfigurationTree::Position m_config_pos;

private:
	//\brief Check the domains of the configured values and do some mappings (e.g. instantiating enum values from strings)
	bool mapValueDomains();
};

}} // _Wolframe::db

#endif // _POSTGRESQL_CONFIG_HPP_INCLUDED
