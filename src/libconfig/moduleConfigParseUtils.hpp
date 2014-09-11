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
/// \brief Utility functions and templates for dynamic config parsing

#ifndef _Wolframe_CONFIGURATION_PARSING_UITILITIES_HPP_INCLUDED
#define _Wolframe_CONFIGURATION_PARSING_UITILITIES_HPP_INCLUDED

#include "config/configurationObject.hpp"
#include "config/configurationTree.hpp"
#include "module/moduleDirectory.hpp"
#include "module/configuredObjectConstructor.hpp"
#include "module/objectDescription.hpp"
#include "logger/logger-v1.hpp"
#include <string>
#include <vector>

namespace {

static bool parseModuleConfiguration( const std::string& configSection, const std::string& configKeyword, const _Wolframe::config::ConfigurationNode& subconfig, std::vector<_Wolframe::config::ConfigurationObject*>& cfgobjs, const _Wolframe::module::ModuleDirectory* modules)
{
	using namespace _Wolframe;
	try
	{
		const module::ConfiguredObjectConstructor* constructor = modules->getConfiguredObjectConstructor( configSection, configKeyword);
		if ( constructor )	{
			config::ConfigurationObject* conf = constructor->configuration();
			if (conf->parse( subconfig, configKeyword, modules))
			{
				cfgobjs.push_back( conf );
			}
			else
			{
				LOG_ERROR << "configuration error in sub section " << configSection << "for structure " << configKeyword << "' " << subconfig.position().logtext();
				delete conf;
				return false;
			}
		}
		else
		{
			LOG_ERROR << "configuration error in sub section " << configSection << ": unknown option or structure: '" << configKeyword << "' " << subconfig.position().logtext();
			return false;
		}
	}
	catch (const std::runtime_error& err)
	{
		LOG_ERROR << "exception parsing configuration in sub section " << configSection << "for structure " << configKeyword << "': " << err.what() << " (" << subconfig.position().logtext() << ")";
		return false;
	}
	return true;
}

}//anonymous namespace
#endif

