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
#include "authenticationFactory.hpp"
#include "standardAuthenticator.hpp"
#include "providerUtils.hpp"
#include "aaaa/authenticator.hpp"
#include "aaaa/passwordChanger.hpp"
#include "database/databaseProviderInterface.hpp"
#include "module/configuredObjectConstructor.hpp"
#include "config/configurationObject.hpp"
#include "logger/logger-v1.hpp"

using namespace _Wolframe;
using namespace _Wolframe::aaaa;

AuthenticationFactory::AuthenticationFactory( const std::vector<config::ConfigurationObject*>& config,
					      system::RandomGenerator* randomGenerator,
					      const module::ModuleDirectory* modules)
{
	if (!createConfiguredProviderObjects( "AuthenticationFactory: " , m_authUnits, config, modules))
	{
		throw std::runtime_error("could not load authentication provider module objects");
	}

	// Iterate through the list of authenticators (built at this point)
	// and build the vector of available mechs and define the random generator
	std::vector<AuthenticationUnit*>::iterator ai = m_authUnits.begin(), ae = m_authUnits.end();
	std::vector<config::ConfigurationObject*>::const_iterator ci = config.begin(), ce = config.end();

	for ( ; ai != ae && ci != ce; ci++,ai++ )
	{
		// init the global random generator
		(*ai)->setRandomGenerator( randomGenerator);

		// add unit mechs to the list
		const char** p_mech = (*ai)->mechs();
		if ( *p_mech == NULL )	{
			LOG_WARNING << "'" << (*ci)->className() << "' has no authentication mechanisms";
		}
		while ( *p_mech )	{
			std::string mech( *p_mech ); boost::to_upper( mech );
			bool exists = false;
			for( std::vector<std::string>::const_iterator mit = m_mechs.begin();
						mit != m_mechs.end(); mit++ )	{
				if ( *mit == mech )
					exists = true;
			}
			if ( ! exists )	{
				m_mechs.push_back( mech );
				LOG_TRACE << "'" << mech << "' authentication mechanism registered";
			}
			p_mech++;
		}
	}
	if (ci != ce || ai != ae) throw std::logic_error("assertion failed in authentication factory: configuration and object lists not parallel");
}

AuthenticationFactory::~AuthenticationFactory()
{
	for ( std::vector< AuthenticationUnit* >::const_iterator it = m_authUnits.begin();
								it != m_authUnits.end(); it++ )
		delete *it;
}

bool AuthenticationFactory::resolveDB( const db::DatabaseProviderInterface& db )
{
	for ( std::vector< AuthenticationUnit* >::const_iterator it = m_authUnits.begin();
								it != m_authUnits.end(); it++ )
		if ( ! (*it)->resolveDB( db ) )
			return false;
	return true;
}


Authenticator* AuthenticationFactory::authenticator( const net::RemoteEndpoint& client) const
{
	return new StandardAuthenticator( m_mechs, m_authUnits, client );
}

PasswordChanger* AuthenticationFactory::passwordChanger( const User& user,
							 const net::RemoteEndpoint& client ) const
{
	for ( std::vector< AuthenticationUnit* >::const_iterator it = m_authUnits.begin();
								it != m_authUnits.end(); it++ )
		if ( boost::iequals( (*it)->id(), user.authenticator()))	{
			PasswordChanger* pwc = (*it)->passwordChanger( user, client );
			if ( pwc )
				return pwc;
		}
	return NULL;
}

