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
///\file testAuthentication.hpp
///\brief Interface of a fake authentication mechanism to test the protocol
#ifndef _Wolframe_MODULE_AUTH_TEST_AUTHENTICATION_HPP_INCLUDED
#define _Wolframe_MODULE_AUTH_TEST_AUTHENTICATION_HPP_INCLUDED

#include "module/moduleInterface.hpp"
#include "module/constructor.hpp"
#include "config/configurationTree.hpp"
#include "serialize/struct/structDescriptionBase.hpp"
#include "AAAA/authenticator.hpp"
#include "AAAA/authUnit.hpp"
#include <boost/lexical_cast.hpp>

namespace _Wolframe {
namespace AAAA {

class TestAuthenticationConfig
	:public config::NamedConfiguration
{
	friend class TestAuthenticationConstructor;
public:
	TestAuthenticationConfig( const char* cfgName, const char* logParent, const char* logName)
		:config::NamedConfiguration( cfgName, logParent, logName) {}

	virtual const char* className() const	
		{return "TestAuthentication";}

	bool parse( const config::ConfigurationNode& pt, const std::string& node, const module::ModulesDirectory* modules);
	bool check() const;
	void print( std::ostream& os, size_t indent) const;
	void setCanonicalPathes( const std::string&){}

public:
	struct Structure
	{
		friend class TestAuthenticationConfigDescription;

		std::string m_identifier;
		std::string m_pattern;

		Structure(){}
		Structure( const std::string& i, const std::string& p)
			:m_identifier(i),m_pattern(p){}

		static const serialize::StructDescriptionBase* getStructDescription();
	};

private:
	friend class TestAuthenticationUnit;
	Structure structure;
	config::ConfigurationTree::Position m_config_pos;
};


class TestAuthenticationUnit
	:public AuthenticationUnit
{
public:
	TestAuthenticationUnit( const TestAuthenticationConfig& cfg);

	~TestAuthenticationUnit(){}

	virtual const char* className() const		{return "TestAuthentication";}

	const std::string* mechs() const;

	AuthenticatorSlice* slice( const std::string& mech, const net::RemoteEndpoint& client);
	
private:
	std::string m_pattern;
};


class TestAuthenticatorSlice
	:public AuthenticatorSlice
{
public:
	TestAuthenticatorSlice( const std::string& identifier_, const std::string& pattern_);

	virtual ~TestAuthenticatorSlice();

	virtual void destroy()					{delete this;}

	virtual const char* className() const			{return "Test";}

	virtual const std::string& identifier() const		{return m_identifier;}

	/// Get the list of available mechs
	virtual const std::vector<std::string>& mechs() const	{return m_mechs;}

	/// Set the authentication mech
	virtual bool setMech( const std::string& mech);

	/// The input message
	virtual void messageIn( const std::string& msg);

	/// The output message
	virtual const std::string& messageOut();

	/// The current status of the authenticator slice
	virtual Status status() const;

	/// The authenticated user or NULL if not authenticated
	virtual User* user() const;

private:
	const std::string& message( const char* cmd, const std::string& content=std::string());
	void initUser( const std::string& msg);

private:
	enum State
	{
		Init,
		StartAuth,
		WaitCredentials,
		Done
	};

	State m_state;
	AuthenticatorSlice::Status m_status;
	std::string m_identifier;
	std::string m_pattern;
	std::string m_message;
	std::vector<std::string> m_mechs;
	User* m_user;
};

}}//namespace
#endif


