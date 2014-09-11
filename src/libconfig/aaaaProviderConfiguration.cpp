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

#include "aaaaProviderConfiguration.hpp"
#include "moduleConfigParseUtils.hpp"
#include "config/configurationObject.hpp"
#include "logger/logger-v1.hpp"
#include "module/moduleDirectory.hpp"
#include "module/simpleObjectConstructor.hpp"
#include "module/configuredObjectConstructor.hpp"
#include "config/valueParser.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::config;

/// constructor
AaaaProviderConfiguration::AaaaProviderConfiguration()
	: config::ConfigurationObject( "AaaaProvider", "AAAA", ""),
	  m_authzDefault( false ), m_mandatoryAudit( true )
{}

/// destructor
AaaaProviderConfiguration::~AaaaProviderConfiguration()
{
	for ( std::vector< config::ConfigurationObject* >::const_iterator it = m_authConfig.begin();
								it != m_authConfig.end(); it++ )
		delete *it;

	for ( std::vector< config::ConfigurationObject* >::const_iterator it = m_authzConfig.begin();
								it != m_authzConfig.end(); it++ )
		delete *it;

	for ( std::vector< config::ConfigurationObject* >::const_iterator it =m_auditConfig.begin();
								it != m_auditConfig.end(); it++ )
		delete *it;
}

bool AaaaProviderConfiguration::parse( const config::ConfigurationNode& pt, const std::string& /*node*/,
			       const module::ModuleDirectory* modules )
{
	using namespace _Wolframe::config;
	bool retVal = true;
	bool authzDfltDefined = false;
	bool mandatoryDefined = false;

	for ( config::ConfigurationNode::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "Authentication" )){
			std::string logStr = logPrefix() + "-authentication: ";
			for ( config::ConfigurationNode::const_iterator L2it = L1it->second.begin();
									L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( "randomDevice", L2it->first ))	{
					LOG_WARNING << "Using deprecated configuration option 'randomDevice' (no effect)";
				}
				else if ( boost::algorithm::iequals( "allowAnonymous", L2it->first ))	{
					LOG_WARNING << "Using deprecated configuration option 'allowAnonymous' (no effect)";
				}
				else if (modules)
				{
					retVal &= parseModuleConfiguration( "Authentication", L2it->first, L2it->second, m_authConfig, modules);
				}
			}
		}
		else if (boost::algorithm::iequals( L1it->first, "Authorization"))	{
			std::string logStr = logPrefix() + "-authorization: ";
			for (config::ConfigurationNode::const_iterator L2it = L1it->second.begin();
									L2it != L1it->second.end(); L2it++ )
			{
				if ( boost::algorithm::iequals( "default", L2it->first ))	{
					if ( ! Parser::getValue( logStr.c_str(), *L2it, m_authzDefault,
								 Parser::BoolDomain(), &authzDfltDefined ))
						retVal = false;
				}
				else if (modules)
				{
					retVal &= parseModuleConfiguration( "Authorization", L2it->first, L2it->second, m_authzConfig, modules);
				}
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "Audit" ))	{
			std::string logStr = logPrefix() + "-audit: ";
			for ( config::ConfigurationNode::const_iterator L2it = L1it->second.begin();
									L2it != L1it->second.end(); L2it++)
			{
				if ( boost::algorithm::iequals( "mandatory", L2it->first ))	{
					if ( ! Parser::getValue( logStr.c_str(), *L2it, m_mandatoryAudit,
								 Parser::BoolDomain(), &mandatoryDefined ))
						retVal = false;
				}
				else if (modules)
				{
					retVal &= parseModuleConfiguration( "Audit", L2it->first, L2it->second, m_auditConfig, modules);
				}
			}
		}
		else
		{
			LOG_ERROR << logPrefix() << "unknown configuration option: '" << L1it->first << "'";
		}
	}
	return retVal;
}


void AaaaProviderConfiguration::print( std::ostream& os, size_t /* indent */ ) const
{
	os << configSection() << std::endl;
	os << "   Authentication" << std::endl;
	os << "      Random number device: " << m_randomDevice << std::endl;
	for ( std::vector< config::ConfigurationObject* >::const_iterator it = m_authConfig.begin();
								it != m_authConfig.end(); it++ )
		(*it)->print( os, 6 );

	os << "   Authorization" << std::endl;
	os << "      Default: " << (m_authzDefault ? "allow" : "deny") << std::endl;
	for ( std::vector< config::ConfigurationObject* >::const_iterator it = m_authzConfig.begin();
								it != m_authzConfig.end(); it++ )
		(*it)->print( os, 6 );

	os << "   Audit" << std::endl;
	os << "      Audit is mandatory: " << (m_mandatoryAudit ? "yes" : "no") << std::endl;
	for ( std::vector< config::ConfigurationObject* >::const_iterator it = m_auditConfig.begin();
								it != m_auditConfig.end(); it++ )
		(*it)->print( os, 6 );

}

/// Check if the AAAA configuration makes sense
bool AaaaProviderConfiguration::check() const
{
	bool correct = true;

	for ( std::vector< config::ConfigurationObject* >::const_iterator it = m_authConfig.begin();
								it != m_authConfig.end(); it++ )	{
		if ( !(*it)->check() )
			correct = false;
	}

	for ( std::vector< config::ConfigurationObject* >::const_iterator it = m_authzConfig.begin();
								it != m_authzConfig.end(); it++ )	{
		if ( !(*it)->check() )
			correct = false;
	}

	for ( std::vector< config::ConfigurationObject* >::const_iterator it = m_auditConfig.begin();
								it != m_auditConfig.end(); it++ )	{
		if ( !(*it)->check() )
			correct = false;
	}

	return correct;
}

void AaaaProviderConfiguration::setCanonicalPathes( const std::string& refPath )
{
	for ( std::vector< config::ConfigurationObject* >::const_iterator it = m_authConfig.begin();
								it != m_authConfig.end(); it++ )
		(*it)->setCanonicalPathes( refPath );

	for ( std::vector< config::ConfigurationObject* >::const_iterator it = m_authzConfig.begin();
								it != m_authzConfig.end(); it++ )
		(*it)->setCanonicalPathes( refPath );


	for ( std::vector< config::ConfigurationObject* >::const_iterator it = m_auditConfig.begin();
								it != m_auditConfig.end(); it++ )
		(*it)->setCanonicalPathes( refPath );
}
