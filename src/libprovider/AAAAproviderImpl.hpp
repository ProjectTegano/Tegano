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

#ifndef _AAAA_PROVIDER_IMPLEMENTATION_HPP_INCLUDED
#define _AAAA_PROVIDER_IMPLEMENTATION_HPP_INCLUDED

#include "AAAA/AAAAproviderInterface.hpp"
#include "authenticationFactory.hpp"
#include "authorizationProvider.hpp"
#include "auditProvider.hpp"
#include "system/randomGenerator.hpp"
#include "module/moduleDirectory.hpp"
#include "database/databaseProvider.hpp"

#include <string>
#include <vector>

namespace _Wolframe {
namespace AAAA {

// AAAA provider implementation
class AAAAproviderImpl
	:public AAAAproviderInterface
{
public:
	AAAAproviderImpl(
		system::RandomGenerator* randomGenerator_,
		const std::vector<config::NamedConfiguration*>& authConfig_,
		const std::vector<config::NamedConfiguration*>& authzConfig_,
		bool authzDefault_,
		const std::vector<config::NamedConfiguration*>& auditConfig_,
		const module::ModuleDirectory* modules);

	~AAAAproviderImpl(){}
	bool resolveDB( const db::DatabaseProvider& db );

	virtual Authenticator* authenticator( const net::RemoteEndpoint& client ) const;
	virtual PasswordChanger* passwordChanger( const User& user, const net::RemoteEndpoint& client) const;
	virtual Authorizer* authorizer() const;
	virtual Auditor* auditor() const;
	virtual system::RandomGenerator* randomGenerator() const;

private:
	system::RandomGenerator* m_randomGenerator;
	AuthenticationFactory	m_authenticator;
	AuthorizationProvider	m_authorizer;
	AuditProvider		m_auditor;
};

}} // namespace _Wolframe::AAAA
#endif // _AAAA_PROVIDER_IMPLEMENTATION_HPP_INCLUDED
