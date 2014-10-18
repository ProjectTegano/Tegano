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
// PAM authentication
//

#ifndef _PAM_AUTHENTICATION_HPP_INCLUDED
#define _PAM_AUTHENTICATION_HPP_INCLUDED

#include "config/configurationObject.hpp"
#include "aaaa/authenticationUnit.hpp"
#include "aaaa/authenticationSlice.hpp"
#include "module/configuredObjectConstructor.hpp"
#include <security/pam_appl.h>
#include <string>

namespace _Wolframe {
namespace aaaa {

class PAMAuthConfig :  public config::ConfigurationObject
{
public:
	PAMAuthConfig( const std::string& className_, const std::string& configSection_, const std::string& configKeyword_)
		: config::ConfigurationObject( className_, configSection_, configKeyword_){}

	bool parse( const config::ConfigurationNode& pt, const std::string& node,
		    const module::ModuleDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;

	const std::string& identifier() const	{return m_identifier;}
	const std::string& service() const	{return m_service;}

private:
	std::string	m_identifier;
	std::string	m_service;
};

// the structure with data we have to pass to the
// PAM callback function (a transport wagon)
typedef struct {
	bool has_login;
	std::string login;
	bool has_pass;
	std::string pass;
	std::string errmsg;
	pam_handle_t *h;
} pam_appdata;

class PAMAuthUnit : public AuthenticationUnit
{
public:
	PAMAuthUnit( const PAMAuthConfig* config);
	PAMAuthUnit( const std::string& identifier_, const std::string& service_);
	~PAMAuthUnit();

	const char** mechs() const;

	AuthenticatorSlice* slice( const std::string& /*mech*/,
				   const net::RemoteEndpoint& /*client*/ );

	User* authenticatePlain( const std::string& username, const std::string& password ) const;
	const std::string& identifier() const	{return m_identifier;}
	const std::string& service() const	{return m_service;}

private:
	std::string m_identifier;
	std::string m_service;		///< name of the PAM service
};


class PAMAuthSlice : public AuthenticatorSlice
{
	enum	SliceState	{
		SLICE_INITIALIZED = 0,		///< Has been initialized, no other data
		SLICE_ASK_FOR_PASSWORD,		///< Ask for password, send 'password?' to the client
		SLICE_WAITING_FOR_PWD,		///< We have sent 'password?' and wait for an answer
		SLICE_USER_NOT_FOUND,		///< User has not been found -> fail
		SLICE_INVALID_CREDENTIALS,	///< Response was wrong -> fail
		SLICE_AUTHENTICATED,		///< Response was correct -> user available
		SLICE_SYSTEM_FAILURE		///< Something is wrong
	};
	
public:
	PAMAuthSlice( const PAMAuthUnit& backend );

	~PAMAuthSlice();

	void dispose();

	virtual const std::string& identifier() const	{ return m_backend.identifier(); }

	/// The input message
	virtual void messageIn( const std::string& message );

	/// The output message
	virtual std::string messageOut();

	/// The current status of the authenticator slice
	virtual Status status() const;

	/// Is the last input message reusable for this mech ?
	virtual bool inputReusable() const		{ return m_inputReusable; }

	/// The authenticated user or NULL if not authenticated
	virtual User* user();

private:
	const PAMAuthUnit&	m_backend;
	SliceState		m_state;
	struct pam_conv		m_conv;		///< PAM internal data structure
	pam_appdata		m_appdata;	///< our void * for PAM data
	bool			m_inputReusable;
	std::string		m_user;
};

}} // namespace _Wolframe::aaaa

#endif // _PAM_AUTHENTICATION_HPP_INCLUDED
