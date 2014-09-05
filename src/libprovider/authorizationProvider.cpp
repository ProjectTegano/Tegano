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
/// \brief Authorization provider

#include "authorizationProvider.hpp"
#include "standardAuthorizer.hpp"
#include "aaaa/authorizationUnit.hpp"
#include <stdexcept>
#include "module/moduleDirectory.hpp"
#include "module/configuredBuilder.hpp"
#include "module/configuredObjectConstructor.hpp"
#include "logger/logger-v1.hpp"
#include "boost/algorithm/string.hpp"

using namespace _Wolframe;
using namespace _Wolframe::aaaa;

AuthorizationProvider::AuthorizationProvider( const std::vector< config::NamedConfiguration* >& confs,
					      bool authzDefault,
					      const module::ModuleDirectory* modules )
{
	for ( std::vector<config::NamedConfiguration*>::const_iterator it = confs.begin();
								it != confs.end(); it++ )	{
		const module::ConfiguredBuilder* builder = modules->getConfiguredBuilder((*it)->className());
		if ( builder )	{
			module::ConfiguredObjectConstructor< AuthorizationUnit >* authz =
					dynamic_cast< module::ConfiguredObjectConstructor< AuthorizationUnit >* >( builder->constructor());
			if ( authz == NULL )	{
				LOG_ALERT << "AuthorizationProvider: '" << builder->objectClassName()
					  << "' is not an Authorization Unit builder";
				throw std::logic_error( "object is not an AuthorizationUnit builder" );
			}
			m_authorizeUnits.push_back( authz->object( **it ) );
			LOG_TRACE << "'" << authz->objectClassName() << "' authorization unit registered";
		}
		else	{
			LOG_ALERT << "AuthorizationProvider: unknown authorization type '" << (*it)->className() << "'";
			throw std::domain_error( "Unknown authorization mechanism type in AuthorizationProvider constructor. See log" );
		}
	}
	m_authorizer = new StandardAuthorizer( m_authorizeUnits, authzDefault );
}

AuthorizationProvider::~AuthorizationProvider()
{
	if ( m_authorizer )
		delete m_authorizer;
	for ( std::vector< AuthorizationUnit* >::iterator it = m_authorizeUnits.begin();
								it != m_authorizeUnits.end(); it++ )
		delete *it;
}

Authorizer* AuthorizationProvider::authorizer() const
{
	return m_authorizer;
}

bool AuthorizationProvider::resolveDB( const db::DatabaseProviderInterface& db )
{
	bool rt = true;
	for ( std::vector< AuthorizationUnit* >::iterator it = m_authorizeUnits.begin();
								it != m_authorizeUnits.end(); it++ )
	{
		if (!(*it)->resolveDB( db))
		{
			LOG_ERROR << "failed to resolve database for authorization unit '" << (*it)->className() << "'";
			rt = false;
		}
	}
	return rt;
}

