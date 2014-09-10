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
// Text file authentication
//

#ifndef _TEXT_FILE_AUTHENTICATION_HPP_INCLUDED
#define _TEXT_FILE_AUTHENTICATION_HPP_INCLUDED

#include "config/configurationObject.hpp"
#include "aaaa/authenticationUnit.hpp"
#include "aaaa/authenticationSlice.hpp"
#include "aaaa/passwordChanger.hpp"
#include "module/configuredObjectConstructor.hpp"
#include "aaaa/user.hpp"
#include "aaaa/CRAM.hpp"
#include "passwdFile.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace aaaa {

class TextFileAuthConfig :  public config::ConfigurationObject
{
public:
	TextFileAuthConfig( const std::string& className_, const std::string& configSection_, const std::string& configKeyword_)
		: config::ConfigurationObject( className_, configSection_, configKeyword_){}

	bool parse( const config::ConfigurationNode& pt, const std::string& node,
		    const module::ModuleDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );

	const std::string& identifier() const	{return m_identifier;}
	const std::string& file() const		{return m_file;}

private:
	std::string			m_identifier;
	std::string			m_file;
};


class TextFileAuthUnit : public AuthenticationUnit
{
public:
	TextFileAuthUnit( const TextFileAuthConfig* config);

	~TextFileAuthUnit();

	const char** mechs() const;

	AuthenticatorSlice* slice( const std::string& mech, const net::RemoteEndpoint& client );
	PasswordChanger* passwordChanger( const User& user, const net::RemoteEndpoint& client );

	/// \brief	Authenticate a user with its plain username and password
	/// \note	This function is supposed to be used only for tests.
	///		DO NOT USE THIS FUNCTION IN REAL AUTHENTICATION MECHANISMS
	///
	/// \param [in]	username
	/// \param [in]	password	guess what this are :D
	/// \param [in]	caseSensitveUser should the username be treated as case-sensitive or not
	User* authenticatePlain( const std::string& username, const std::string& password ) const;

	/// \brief	Get the user data
	/// \param [in]	hash		username hash (base64 of the HMAC-SHA256)
	/// \param [in]	key		the key used for computing the username hash (base64)
	/// \param [out] user		reference to the user structure that will be filled
	///				by the function in case of success
	/// \param [in]	caseSensitveUser should the username be treated as case-sensitive or not
	/// \returns	true if the user was found, false otherwise
	bool getUser( const std::string& hash, const std::string& key, PwdFileUser& user ) const;
	bool getUser( const std::string& userHash, PwdFileUser& user ) const;

	bool getUserPlain( const std::string& username, PwdFileUser& user ) const;
	const std::string& identifier() const	{return m_identifier;}

private:
	static const std::string	m_mechs[];
	std::string			m_identifier;
	const PasswordFile		m_pwdFile;
};


// Flow:
// Initialize --> receive username key + HMAC --> + user found --> send salt + challenge --> (*)
//                                                + user not found --> finish
//
// (*) --> receive response --> + got user
//				+ invalid credentials
//
class TextFileAuthSlice : public AuthenticatorSlice
{
	enum	SliceState	{
		SLICE_INITIALIZED,		///< Has been initialized, no other data
		SLICE_USER_FOUND,		///< User has been found, will send challenge
		SLICE_USER_NOT_FOUND,		///< User has not been found -> fail
		SLICE_CHALLENGE_SENT,		///< Waiting for the response
		SLICE_INVALID_CREDENTIALS,	///< Response was wrong -> fail
		SLICE_AUTHENTICATED,		///< Response was correct -> user available
		SLICE_SYSTEM_FAILURE		///< Something is wrong
	};

public:
	TextFileAuthSlice( const TextFileAuthUnit& backend );

	~TextFileAuthSlice();

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

	/// Tell the slice that it is the last one
	virtual void lastSlice()			{ m_lastSlice = true; }

	/// The authenticated user or NULL if not authenticated
	virtual User* user();

private:
	const TextFileAuthUnit&	m_backend;
	struct PwdFileUser	m_usr;
	SliceState		m_state;
	CRAMchallenge*		m_challenge;
	bool			m_inputReusable;
	bool			m_lastSlice;
	bool			m_fakeUser;
};


// Flow:
// Initialize --> send salt + challenge -->
//  --> client encrypts the new password + password crc with the response -->
//  --> server decrypts with the response --> crc matches --> + - yes --> change pwd
//                                                            + - no  --> abort
//
class TextFilePwdChanger : public PasswordChanger
{
	enum	ChangerState	{
		CHANGER_INITIALIZED,		///< Has been initialized, no other data
		CHANGER_CHALLENGE_SENT,		///< Waiting for the answer
		CHANGER_INVALID_MESSAGE,	///< Answer CRC was wrong -> fail
		CHANGER_PASSWORD_EXCHANGED,	///< Answer CRC was correct -> password changed
		CHANGER_SYSTEM_FAILURE		///< Something is wrong
	};

	static const char* statusName( ChangerState i )
	{
		static const char* ar[] = {	"CHANGER_INITIALIZED",
						"CHANGER_CHALLENGE_SENT",
						"CHANGER_INVALID_MESSAGE",
						"CHANGER_PASSWORD_EXCHANGED",
						"CHANGER_SYSTEM_FAILURE"
					  };
		return ar[ i ];
	}

public:
	TextFilePwdChanger( const TextFileAuthUnit& backend, const std::string& username );

	~TextFilePwdChanger();

	void dispose();

	virtual const std::string& identifier() const	{ return m_backend.identifier(); }

	/// The input message
	virtual void messageIn( const std::string& message );

	/// The output message
	virtual std::string messageOut();

	/// The current status of the authenticator slice
	virtual Status status() const;

	/// The new password
	virtual std::string password();

private:
	const TextFileAuthUnit&	m_backend;
	struct PwdFileUser	m_usr;
	ChangerState		m_state;
	CRAMchallenge*		m_challenge;
	std::string		m_password;
};

}} // namespace _Wolframe::aaaa

#endif // _TEXT_FILE_AUTHENTICATION_HPP_INCLUDED
