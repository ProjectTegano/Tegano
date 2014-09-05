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
/// \brief AAAA provider implementation

#include "aaaaProviderImpl.hpp"
#include "logger/logger-v1.hpp"
#include "authenticationFactory.hpp"
#include "auditProvider.hpp"
#include "authorizationProvider.hpp"
#include "system/randomGenerator.hpp"

using namespace _Wolframe;
using namespace _Wolframe::aaaa;

AaaaProviderImpl::AaaaProviderImpl(
		system::RandomGenerator* randomGenerator_,
		const std::vector<config::NamedConfiguration*>& authConfig_,
		const std::vector<config::NamedConfiguration*>& authzConfig_,
		bool authzDefault_,
		const std::vector<config::NamedConfiguration*>& auditConfig_,
		const module::ModuleDirectory* modules_)
	:m_randomGenerator(randomGenerator_)
	,m_authenticator( authConfig_, randomGenerator_, modules_)
	,m_authorizer( authzConfig_, authzDefault_, modules_)
	,m_auditor( auditConfig_, modules_)
{}

bool AaaaProviderImpl::resolveDB( const db::DatabaseProviderInterface& db )
{
	LOG_DATA << "Resolving authentication databases";
	if ( !m_authenticator.resolveDB( db ))
		return false;
	LOG_DATA << "Resolving authorization databases";
	if ( !m_authorizer.resolveDB( db ))
		return false;
	LOG_DATA << "Resolving audit databases";
	if ( !m_auditor.resolveDB( db ))
		return false;
	LOG_TRACE << "AAAA database references resolved";
	return true;
}

Authenticator* AaaaProviderImpl::authenticator( const net::RemoteEndpoint& client ) const
{
	return m_authenticator.authenticator( client );
}

PasswordChanger* AaaaProviderImpl::passwordChanger( const User& user,
				  const net::RemoteEndpoint& client ) const
{
	return m_authenticator.passwordChanger( user, client );
}

Authorizer* AaaaProviderImpl::authorizer() const
{
	return m_authorizer.authorizer();
}

Auditor* AaaaProviderImpl::auditor() const
{
	return m_auditor.auditor();
}

system::RandomGenerator* AaaaProviderImpl::randomGenerator() const
{
	return m_randomGenerator;
}


