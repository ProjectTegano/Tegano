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
/// \brief Database provider configuration functions

#include "databaseProviderConfiguration.hpp"
#include "moduleConfigParseUtils.hpp"
#include "config/valueParser.hpp"
#include "config/configurationTree.hpp"
#include "config/configurationObject.hpp"
#include "module/moduleDirectory.hpp"
#include "module/simpleObjectConstructor.hpp"
#include "module/configuredObjectConstructor.hpp"
#include "logger/logger-v1.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <ostream>
#include <string>
#include <cassert>
#include <vector>

using namespace _Wolframe;
using namespace _Wolframe::config;

bool DatabaseProviderConfiguration::parse(
			const config::ConfigurationNode& pt,
			const std::string& /*node*/,
			const module::ModuleDirectory* modules )
{
	using namespace _Wolframe::config;
	bool retVal = true;

	for (config::ConfigurationNode::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++)
	{
		if (modules)
		{
			retVal &= parseModuleConfiguration( "Database", L1it->first, L1it->second, m_config, modules);
		}
	}
	return retVal;
}

DatabaseProviderConfiguration::~DatabaseProviderConfiguration()
{
	for ( std::vector< config::ConfigurationObject* >::const_iterator it = m_config.begin();
								it != m_config.end(); it++ )
		delete *it;
}

void DatabaseProviderConfiguration::print( std::ostream& os, size_t /* indent */ ) const
{
	os << configSection() << std::endl;
	if ( m_config.size() > 0 )	{
		for ( std::vector< config::ConfigurationObject* >::const_iterator it = m_config.begin();
								it != m_config.end(); it++ )	{
			(*it)->print( os, 3 );
		}
	}
	else
		os << "   None configured" << std::endl;
}


/// Check if the database configuration makes sense
bool DatabaseProviderConfiguration::check() const
{
	bool correct = true;
	for ( std::vector< config::ConfigurationObject* >::const_iterator it = m_config.begin();
								it != m_config.end(); it++ )	{
		if ( !(*it)->check() )
			correct = false;
	}
	return correct;
}

void DatabaseProviderConfiguration::setCanonicalPathes( const std::string& refPath )
{
	for ( std::vector< config::ConfigurationObject* >::const_iterator it = m_config.begin();
								it != m_config.end(); it++ )	{
		(*it)->setCanonicalPathes( refPath );
	}
}

