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
/// \brief Authentication factory

#ifndef _AAAA_AUTHENTICATION_FACTORY_HPP_INCLUDED
#define _AAAA_AUTHENTICATION_FACTORY_HPP_INCLUDED

#include "database/DBprovider.hpp"
#include "config/configurationBase.hpp"
#include "system/connectionEndpoint.hpp"
#include "system/randomGenerator.hpp"
#include "module/moduleDirectory.hpp"
#include "AAAA/user.hpp"
#include "AAAA/authenticationUnit.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace AAAA {

class Authenticator;
class PasswordChanger;

class AuthenticationFactory
{
public:
	AuthenticationFactory( const std::vector< config::NamedConfiguration* >& confs,
			       system::RandomGenerator* randomGenerator,
			       const module::ModuleDirectory* modules);
	~AuthenticationFactory();
	bool resolveDB( const db::DatabaseProvider& db );

	Authenticator* authenticator( const net::RemoteEndpoint& client) const;
	PasswordChanger* passwordChanger( const User& user,
					  const net::RemoteEndpoint& client) const;
private:
	std::vector< AuthenticationUnit* >	m_authUnits;
	std::vector< std::string >		m_mechs;
};

}}//namespace 
#endif

