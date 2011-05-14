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
// authentication_pam.cpp
//

#include "AAAA/authentication_pam.hpp"

#ifdef WITH_PAM

#include <string.h>
#include <stdlib.h>

#include <sstream>
#include <stdexcept>

namespace _Wolframe {
namespace AAAA {

static int pam_conv_func(	int nmsg, const struct pam_message **msg,
				struct pam_response **reply, void *appdata_ptr );

PAMAuthenticator::PAMAuthenticator( const std::string _service )
	: m_service( _service )
{
	m_state = _Wolframe_PAM_STATE_NEED_LOGIN;
	m_appdata.h = NULL;
	m_appdata.has_pass = false;
	m_appdata.pass = "";
	m_conv.conv = pam_conv_func;
	m_conv.appdata_ptr = &m_appdata;
}

static const char *msg_style_to_str( int msg_style )
{
	switch( msg_style ) {
		case PAM_PROMPT_ECHO_OFF:	return "PAM_PROMPT_ECHO_OFF";
		case PAM_TEXT_INFO:		return "PAM_TEXT_INFO";
		case PAM_ERROR_MSG:		return "PAM_ERROR_MSG";
		case PAM_PROMPT_ECHO_ON:	return "PAM_PROMPT_ECHO_ON";
		default:			return "<unknown msg type>";
	}
}

static void null_and_free( int nmsg, struct pam_response *pr )
{
	int i;
	struct pam_response *r = pr;

	if( pr == NULL ) return;

	for( i = 0; i < nmsg; i++, r++ ) {
		if( r->resp != NULL ) {
			memset( r->resp, 0, strlen( r->resp ) );
			free( r->resp );
			r->resp = NULL;
		}
	}
	free( pr );
}

static int pam_conv_func(	int nmsg, const struct pam_message **msg,
				struct pam_response **reply, void *appdata_ptr )
{
	int i;
	const struct pam_message *m = *msg;
	struct pam_response *r;
	int rc;
	const char *login_prompt = NULL;
	union { const char *s; const void *v; } login_prompt_union;

	pam_appdata *appdata = (pam_appdata *)appdata_ptr;

	login_prompt_union.s = login_prompt;

// check sanity of the messages passed
	if( nmsg <= 0 || nmsg >= PAM_MAX_NUM_MSG ) {
		std::ostringstream ss;
		ss	<< "Bad number of messages in PAM conversion function: " << nmsg
			<< "must be between " << nmsg << " and " << PAM_MAX_NUM_MSG;
		appdata->errmsg = ss.str( );
		*reply = NULL;
		return PAM_CONV_ERR;
	}

// allocate return messages
	if( ( *reply = r = (struct pam_response *)calloc( nmsg, sizeof( struct pam_response ) ) ) == NULL ) {
		appdata->errmsg = "Unable to allocate memory for replies";
		return PAM_BUF_ERR;
	}

	for( i = 0; i < nmsg; i++ ) {
		if( msg[i]->msg == NULL ) {
			std::ostringstream ss;

			ss	<< "Bad message number " << i
				<< " of type " << msg_style_to_str( m->msg_style )
				<< " is not supposed to be NULL!";
			appdata->errmsg = ss.str( );
			goto error;
		}

// initalize response
		r->resp = NULL;
		r->resp_retcode = 0;

		switch( msg[i]->msg_style ) {
// Usually we get prompted for a password, this is not always true though.
			case PAM_PROMPT_ECHO_OFF:
// thank you very much, come again (but with a password)
				if( !appdata->has_pass )
					return PAM_CONV_AGAIN;

				r->resp = strdup( appdata->pass.c_str( ) );
				if( r->resp == NULL ) {
					appdata->errmsg = "Unable to allocate memory for password answer";
					goto error;
				}
				break;

// Check against the PAM_USER_PROMPT to be sure we have a login request.
// Always recheck because the library could change the prompt any time
			case PAM_PROMPT_ECHO_ON:
				rc = pam_get_item( appdata->h, PAM_USER_PROMPT, &login_prompt_union.v );
				if( rc != PAM_SUCCESS ) {
					std::ostringstream ss;

					ss	<< "pam_get_item( PAM_USER_PROMPT) failed with error "
						<< pam_strerror( appdata->h, rc ) << "(" << rc << ")";
					appdata->errmsg = ss.str( );
					goto error;
				}
				if( strcmp( m->msg, login_prompt ) == 0 ) {
					r->resp = strdup( appdata->login.c_str( ) );
					if( r->resp == NULL ) {
						appdata->errmsg = "Unable to allocate memory for login answer";
						goto error;
					}
				}
				break;

// internal pam errors, infos (TODO: log later)
			case PAM_ERROR_MSG:
			case PAM_TEXT_INFO:
				break;

			default: {
				std::ostringstream ss;
				ss	<< "Unknown message " << i << " of type "
					<< msg_style_to_str( m->msg_style ) << ": "
					<< m->msg;
				appdata->errmsg = ss.str( );
				goto error;
			}

// next message and response
			m++; r++;
		}
	}

	return PAM_SUCCESS;

error:
	null_and_free( i, r );
	*reply = NULL;
	return PAM_CONV_ERR;
}

Step::AuthStep PAMAuthenticator::nextStep( )
{
	int rc = 0;

	switch( m_state ) {
		case _Wolframe_PAM_STATE_NEED_LOGIN:
			m_token = "login";
			return Step::_Wolframe_AUTH_STEP_RECV_DATA;

		case _Wolframe_PAM_STATE_HAS_LOGIN:
// TODO: the service name must be a CONSTANT due to security reasons!
			rc = pam_start( m_service.c_str( ), m_appdata.login.c_str( ), &m_conv, &m_appdata.h );
			if( rc != PAM_SUCCESS ) {
				std::ostringstream ss;
				ss	<< "pam_start failed with service " << m_service << ": "
					<< pam_strerror( m_appdata.h, rc ) << " (" << rc << ")";
				throw std::runtime_error( ss.str( ) );
			}
// everything went ok, so authentication succeeded with for example pam_rootok.so withou
// requiring a password.. intentionally no break here :-)

// authenticate: are we who we claim to be?
		case _Wolframe_PAM_STATE_HAS_PASS:
			rc = pam_authenticate( m_appdata.h, 0 );
			if( m_state == _Wolframe_PAM_STATE_HAS_LOGIN && rc == PAM_INCOMPLETE ) {
// we need a password, so drop out of authentication, keep the state and return in
// _Wolframe_PAM_STATE_HAS_PASS..
				m_token = "password";
				m_state = _Wolframe_PAM_STATE_NEED_PASS;
				return Step::_Wolframe_AUTH_STEP_RECV_DATA;
			} else if( rc != PAM_SUCCESS ) {
				std::ostringstream ss;
				ss	<< "pam_authenticate failed with service " << m_service << ": "
					<< pam_strerror( m_appdata.h, rc ) << " (" << rc << ")";
				m_error = ss.str( );
				m_state = _Wolframe_PAM_STATE_ERROR;
				return Step::_Wolframe_AUTH_STEP_GET_ERROR;
			}

// is access to the account permitted?
			rc = pam_acct_mgmt( m_appdata.h, 0 );
			if( rc != PAM_SUCCESS ) {
				std::ostringstream ss;
				ss	<< "pam_acct_mgmt failed with service " << m_service << ": "
					<< pam_strerror( m_appdata.h, rc ) << " (" << rc << ")";
				m_error = ss.str( );
				m_state = _Wolframe_PAM_STATE_ERROR;
				return Step::_Wolframe_AUTH_STEP_GET_ERROR;
			}

// terminate PAM session with last exit code
			if( pam_end( m_appdata.h, rc ) != PAM_SUCCESS ) {
				std::ostringstream ss;
				ss	<< "pam_end failed with service " << m_service << ": "
					<< pam_strerror( m_appdata.h, rc ) << " (" << rc << ")";
				(void)pam_end( m_appdata.h, rc );
				throw std::runtime_error( ss.str( ) );
			}

			m_state = _Wolframe_PAM_STATE_NEED_LOGIN;
			m_appdata.h = NULL;
			m_appdata.pass = "";

			if( rc == PAM_SUCCESS )
				return Step::_Wolframe_AUTH_STEP_SUCCESS;
			else
				return Step::_Wolframe_AUTH_STEP_FAIL;

		case _Wolframe_PAM_STATE_ERROR:
			(void)pam_end( m_appdata.h, rc );
			m_appdata.h = NULL;
			m_appdata.has_pass = false;
			m_appdata.pass = "";
			m_state = _Wolframe_PAM_STATE_NEED_LOGIN;
			return Step::_Wolframe_AUTH_STEP_FAIL;

		case _Wolframe_PAM_STATE_NEED_PASS:
			throw new std::runtime_error( "Illegal state in PAMAuthenticator::nextStep!" );
			break;

		default:
			throw new std::runtime_error( "Unknown state in PAMAuthenticator::nextStep!" );
			break;
	}

	return Step::_Wolframe_AUTH_STEP_FAIL;
}

std::string PAMAuthenticator::sendData( )
{
	switch( m_state ) {
		case _Wolframe_PAM_STATE_ERROR:
			return m_data;

		case _Wolframe_PAM_STATE_NEED_LOGIN:
		case _Wolframe_PAM_STATE_HAS_LOGIN:
		case _Wolframe_PAM_STATE_NEED_PASS:
		case _Wolframe_PAM_STATE_HAS_PASS:
			throw new std::runtime_error( "Illegal state in PAMAuthenticator::sendData!" );
			break;

		default:
			throw new std::runtime_error( "Unknown state in PAMAuthenticator::sendData!" );
			break;
	}

	return 0;
}

std::string PAMAuthenticator::token( )
{
	return m_token;
}

void PAMAuthenticator::receiveData( const std::string data )
{
	switch( m_state ) {
		case _Wolframe_PAM_STATE_NEED_LOGIN:
			m_appdata.login = data;
			m_state = _Wolframe_PAM_STATE_HAS_LOGIN;
			break;

		case _Wolframe_PAM_STATE_NEED_PASS:
			m_appdata.has_pass = true;
			m_appdata.pass = data;
			m_state = _Wolframe_PAM_STATE_HAS_PASS;
			break;

// TODO: application exception
		case _Wolframe_PAM_STATE_HAS_LOGIN:
		case _Wolframe_PAM_STATE_HAS_PASS:
		case _Wolframe_PAM_STATE_ERROR:
			throw new std::runtime_error( "Illegal state in PAMAuthenticator::receiveData!" );
			break;

		default:
			throw new std::runtime_error( "Unknown state in PAMAuthenticator::receiveData!" );
			break;
	}
}

std::string PAMAuthenticator::getError( )
{
	return m_error;
}

}} // namespace _Wolframe::AAAA

#endif // WITH_PAM
