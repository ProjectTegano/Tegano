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
//
// Wolframe processor provider
//

#include "procProviderConfiguration.hpp"
#include "moduleConfigParseUtils.hpp"
#include "config/valueParser.hpp"
#include "config/configurationTree.hpp"
#include "config/configurationObject.hpp"
#include "module/simpleObjectConstructor.hpp"
#include "module/configuredObjectConstructor.hpp"
#include "logger/logger-v1.hpp"
#include "utils/fileUtils.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <ostream>
#include <string>

using namespace _Wolframe;
using namespace _Wolframe::config;

bool ProcProviderConfiguration::parse( const config::ConfigurationNode& pt, const std::string& /*node*/,
				const module::ModuleDirectory* modules )
{
	using namespace _Wolframe::config;
	bool retVal = true;

	for ( config::ConfigurationNode::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( "database", L1it->first ))
		{
			std::string databaseId;
			if ( ! Parser::getValue( logPrefix().c_str(), *L1it, databaseId))
			{
				retVal = false;
			}
			else
			{
				m_databaseIds.push_back( databaseId);
			}
		}
		else if ( boost::algorithm::iequals( "program", L1it->first ) )	{
			std::string programFile;
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, programFile ))
			{
				retVal = false;
			}
			else
			{
				m_programFiles.push_back( programFile );
			}
		}
		else if ( boost::algorithm::iequals( "cmdhandler", L1it->first )
			|| boost::algorithm::iequals( "runtimeenv", L1it->first ) )	{
			for ( config::ConfigurationNode::const_iterator L2it = L1it->second.begin();
									  L2it != L1it->second.end(); L2it++ )	{
				if (modules)
				{
					retVal &= parseModuleConfiguration( L1it->first, L2it->first, L2it->second, m_procConfig, modules);
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

ProcProviderConfiguration::~ProcProviderConfiguration()
{
	for ( std::vector< config::ConfigurationObject* >::const_iterator it = m_procConfig.begin();
								it != m_procConfig.end(); it++ )
		delete *it;
}

void ProcProviderConfiguration::print( std::ostream& os, size_t indent ) const
{
	os << configSection() << std::endl;
	std::vector<std::string>::const_iterator di = m_databaseIds.begin(), de = m_databaseIds.end();
	for (; di != de; ++di)
	{
		os << "   Database: " << *di << std::endl;
	}
	if ( m_procConfig.size() > 0 )	{
		for ( std::vector< config::ConfigurationObject* >::const_iterator it = m_procConfig.begin();
								it != m_procConfig.end(); it++ )	{
			(*it)->print( os, 3 );
		}
	}
	else
		os << "   None configured" << std::endl;

	std::string indStr( indent + 1, '\t');
	if ( m_programFiles.size() == 0 )
		os << "   Program file: none" << std::endl;
	else if ( m_programFiles.size() == 1 )
		os << "   Program file: " << m_programFiles.front() << std::endl;
	else	{
		std::vector< std::string >::const_iterator it = m_programFiles.begin();
		os << "   Program files: " << *it++ << std::endl;
		while ( it != m_programFiles.end() )
			os << "                  " << *it++ << std::endl;
	}
}


/// Check if the configuration makes sense
bool ProcProviderConfiguration::check() const
{
	bool correct = true;

	for ( std::vector< config::ConfigurationObject* >::const_iterator it = m_procConfig.begin();
								it != m_procConfig.end(); it++ )	{
		if ( !(*it)->check() )
			correct = false;
	}
	return correct;
}

void ProcProviderConfiguration::setCanonicalPathes( const std::string& refPath )
{
	m_referencePath = refPath;
	for ( std::vector< config::ConfigurationObject* >::const_iterator it = m_procConfig.begin();
								it != m_procConfig.end(); it++ )	{
		(*it)->setCanonicalPathes( refPath );
	}
	for ( std::vector< std::string >::iterator it = m_programFiles.begin();
						it != m_programFiles.end(); it++ )	{
		std::string oldPath = *it;
		*it = utils::getCanonicalPath( *it, refPath );
		if ( oldPath != *it )	{
/*MBa ?!?*/		LOG_NOTICE << logPrefix() << "Using program absolute filename '" << *it
				   << "' instead of '" << oldPath << "'";
		}
	}
}

