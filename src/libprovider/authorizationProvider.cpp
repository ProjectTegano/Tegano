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
#include "providerUtils.hpp"
#include "standardAuthorizer.hpp"
#include "aaaa/authorizationUnit.hpp"
#include <stdexcept>
#include "module/moduleDirectory.hpp"
#include "module/configuredObjectConstructor.hpp"
#include "config/configurationObject.hpp"
#include "logger/logger-v1.hpp"
#include "boost/algorithm/string.hpp"

using namespace _Wolframe;
using namespace _Wolframe::aaaa;

AuthorizationProvider::AuthorizationProvider( const std::vector< config::ConfigurationObject* >& config,
					      bool authzDefault,
					      const module::ModuleDirectory* modules )
{
	if (!createConfiguredProviderObjects( "AuthorizationProvider: ", m_authorizeUnits, config, modules))
	{
		throw std::runtime_error("could not load authentication provider module objects");
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
			LOG_ERROR << "failed to resolve database for authorization unit '" << (*it)->id() << "'";
			rt = false;
		}
	}
	return rt;
}

