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
// authentication_pam.hpp
//

#ifndef _Wolframe_AUTHENTICATION_PAM_HPP_INCLUDED
#define _Wolframe_AUTHENTICATION_PAM_HPP_INCLUDED

#include "AAAA/authentication.hpp"

#ifdef WITH_PAM

#include <string>

#include <security/pam_appl.h>

namespace _Wolframe {
namespace AAAA {


typedef struct {
	std::string login;
	bool has_pass;
	std::string pass;
	std::string errmsg;
	pam_handle_t *h;
} pam_appdata;

class PAMAuthenticator : public Authenticator {
	private:
		std::string m_service;

		// states of the authenticator state machine
		enum {
			_Wolframe_PAM_STATE_NEED_LOGIN,
			_Wolframe_PAM_STATE_HAS_LOGIN,
			_Wolframe_PAM_STATE_NEED_PASS,
			_Wolframe_PAM_STATE_HAS_PASS,
			_Wolframe_PAM_STATE_ERROR
		} m_state;

		pam_appdata m_appdata;
		std::string m_token;
		std::string m_data;
		std::string m_error;
		struct pam_conv m_conv;

	public:
		PAMAuthenticator( const std::string _service );
		virtual Step::AuthStep nextStep( );
		virtual std::string sendData( );
		virtual std::string token( );
		virtual void receiveData( const std::string data );
		virtual std::string getError( );
};

}} // namespace _Wolframe::AAAA

#endif // WITH_PAM

#endif // _Wolframe_AUTHENTICATION_PAM_HPP_INCLUDED
