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
/// \file moduleDirectoryImpl.cpp

#include "moduleDirectoryImpl.hpp"
#include "moduleLoader.hpp"
#include "module/configuredBuilder.hpp"
#include "module/simpleBuilder.hpp"
#include "utils/fileUtils.hpp"
#include <boost/algorithm/string.hpp>
#include "logger/logger-v1.hpp"

using namespace _Wolframe;
using namespace _Wolframe::module;

bool ModuleDirectoryImpl::addBuilder( const ConfiguredBuilder* builder )
{
	for ( std::vector< const ConfiguredBuilder* >::const_iterator it = m_cfgdBuilder.begin();
							it != m_cfgdBuilder.end(); it++ )	{
		if ( boost::algorithm::iequals( (*it)->section(), builder->section() ) &&
				boost::algorithm::iequals( (*it)->keyword(), builder->keyword() ))	{
			LOG_ALERT << "A configuration module for section '" << builder->section()
				  << "' keyword '" << builder->keyword() << "' already exists";
			return false;
		}
		if ( boost::algorithm::iequals( (*it)->className(), builder->className() ))	{
			LOG_ALERT << "A module container named '" << builder->className()
				  << "' already exists";
			return false;
		}
	}
	m_cfgdBuilder.push_back( builder );
	LOG_DEBUG << "Module '" << builder->className() << "' registered for section '"
		  << builder->section() << "' keyword '" << builder->keyword() << "'";
	return true;
}

bool ModuleDirectoryImpl::addBuilder( const SimpleBuilder* builder )
{
	for ( std::vector< const SimpleBuilder* >::const_iterator it = m_simpleBuilder.begin();
							it != m_simpleBuilder.end(); it++ )	{
		if ( boost::algorithm::iequals( (*it)->className(), builder->className() ))	{
			LOG_ALERT << "A module object named '" << builder->className()
				  << "' already exists";
			return false;
		}
	}
	m_simpleBuilder.push_back( builder );
	LOG_DEBUG << "Module object '" << builder->className() << "' registered";
	return true;
}

const ConfiguredBuilder* ModuleDirectoryImpl::getConfiguredBuilder( const std::string& section,
						 const std::string& keyword ) const
{
	for ( std::vector< const ConfiguredBuilder* >::const_iterator it = m_cfgdBuilder.begin();
							it != m_cfgdBuilder.end(); it++ )	{
		if ( boost::algorithm::iequals( (*it)->keyword(), keyword ) &&
				boost::algorithm::iequals( (*it)->section(), section ))
			return *it;
	}
	return NULL;
}

const ConfiguredBuilder* ModuleDirectoryImpl::getConfiguredBuilder( const std::string& objectClassName ) const
{
	for ( std::vector< const ConfiguredBuilder* >::const_iterator it = m_cfgdBuilder.begin();
							it != m_cfgdBuilder.end(); it++ )
	{
		LOG_DATA << "Get builder of '" << objectClassName << "' test '" << (*it)->className() << "'";
		if ( boost::algorithm::iequals( (*it)->className(), objectClassName ))
		{
			return *it;
		}
	}
	return NULL;
}

const std::vector<const SimpleBuilder*>& ModuleDirectoryImpl::getSimpleBuilderList() const
{
	return m_simpleBuilder;
}

#define DO_STRINGIFY(x)	#x
#define STRINGIFY(x)	DO_STRINGIFY(x)

#if defined( DEFAULT_MODULE_LOAD_DIR )
std::string ModuleDirectoryImpl::getAbsoluteModulePath( const std::string& moduleName, const std::string& configuredDirectory, bool useDefaultModuleDir) const
#else
std::string ModuleDirectoryImpl::getAbsoluteModulePath( const std::string& moduleName, const std::string& configuredDirectory, bool) const
#endif
{
	// Add the module extension, if not defined
	std::string moduleName_ = moduleName;
#if defined(_WIN32)
	if (utils::getFileExtension( moduleName).empty())
	{
		moduleName_.append( ".dll");
	}
#else
	if (utils::getFileExtension( moduleName).empty())
	{
		moduleName_.append( ".so");
	}
#endif
	// Building up list of module directories ascending in order of picking priority:
	std::vector<std::string> pathpriolist;
	if (!configuredDirectory.empty())
	{
		std::string configuredDirectoryAbsolute = utils::getCanonicalPath( configuredDirectory, m_confDir);
		pathpriolist.push_back( configuredDirectoryAbsolute);
	}
#if defined( DEFAULT_MODULE_LOAD_DIR )
	if (useDefaultModuleDir)
	{
		pathpriolist.push_back( std::string( STRINGIFY( DEFAULT_MODULE_LOAD_DIR )));
	}
#endif
	pathpriolist.push_back( m_confDir);

	// Find the first match of an existing file in the module path priority list:
	std::vector<std::string>::const_iterator pi = pathpriolist.begin(), pe = pathpriolist.end();
	for (; pi != pe; ++pi)
	{
		std::string modulePathAbsolute = utils::getCanonicalPath( moduleName_, *pi);
		if (utils::fileExists( modulePathAbsolute))
		{
			return modulePathAbsolute;
		}
	}

	// Return empty string, if not found
	return std::string();
}

bool module::ModuleDirectoryImpl::loadModules( const std::vector< std::string >& modFiles)
{
	bool retVal = true;

	for ( std::vector< std::string >::const_iterator it = modFiles.begin();
							it != modFiles.end(); it++ )
	{
		ModuleEntryPoint* entry = loadModuleEntryPoint( *it);
		if ( !entry )
		{
			LOG_ERROR << "Failed to load module '" << *it << "'";
			retVal = false;
			break;
		}
		for ( unsigned short i = 0; entry->getBuilder[ i ]; i++ )
		{
			const BuilderBase* builder = entry->getBuilder[ i ]();
			const SimpleBuilder* simpleBuilder = dynamic_cast<const SimpleBuilder*>(builder);
			const ConfiguredBuilder* configuredBuilder = dynamic_cast<const ConfiguredBuilder*>(builder);
			if (configuredBuilder)
			{
				addBuilder( configuredBuilder);
			}
			else if (simpleBuilder)
			{
				addBuilder( simpleBuilder);
			}
			else
			{
				LOG_ERROR << "Unknown type of builder in module '" << entry->name << "'";
			}
		}
		LOG_DEBUG << "Module '" << entry->name << "' loaded";
	}
	return retVal;
}
