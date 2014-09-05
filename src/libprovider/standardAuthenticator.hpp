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
/// \brief Standard authenticator

#ifndef _AAAA_STANDARD_AUTHENTICATOR_HPP_INCLUDED
#define _AAAA_STANDARD_AUTHENTICATOR_HPP_INCLUDED

#include "aaaa/authenticator.hpp"
#include "aaaa/authenticationUnit.hpp"
#include "aaaa/authenticationSlice.hpp"

#include <string>
#include <vector>

namespace _Wolframe {
namespace aaaa {

// Standard authentication class and authentication provider
class StandardAuthenticator : public Authenticator
{
public:
	StandardAuthenticator( const std::vector<std::string>& mechs_,
			       const std::vector< AuthenticationUnit* >& units_,
			       const net::RemoteEndpoint& client_ );

	~StandardAuthenticator();
	void dispose();

	/// Get the list of available mechs
	virtual const std::vector<std::string>& mechs() const;

	/// Set the authentication mech
	virtual bool setMech( const std::string& mech );

	/// The input message
	virtual void messageIn( const std::string& message );

	/// The output message
	virtual std::string messageOut();

	/// The current status of the authenticator
	virtual Status status() const		{ return m_status; }

	/// The authenticated user or NULL if not authenticated
	virtual User* user();
private:
	const std::vector< std::string >&		m_mechs;
	const std::vector< AuthenticationUnit* >&	m_authUnits;
	const net::RemoteEndpoint&			m_client;

	Authenticator::Status				m_status;
	std::vector< AuthenticatorSlice* >		m_slices;
	int						m_currentSlice;
	aaaa::User*					m_user;
};

}}//namespace
#endif

