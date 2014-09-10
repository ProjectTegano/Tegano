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
/// \file moduleDirectoryImpl.hpp
/// \brief Module objects directory implementation

#ifndef _MODULE_DIRECTORY_IMPLEMENTATION_HPP_INCLUDED
#define _MODULE_DIRECTORY_IMPLEMENTATION_HPP_INCLUDED
#include "module/moduleDirectory.hpp"
#include "module/simpleObjectConstructor.hpp"
#include "module/configuredObjectConstructor.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace module {

/// \class ModuleDirectory
/// \brief The modules directory with all modules loaded
class ModuleDirectoryImpl
	:public module::ModuleDirectory
{
public:
	/// \brief Constructor
	/// \param[in] confDir_ Configuration directory that specifies the relative path where to load the modules from if not specified by the system or explicitely
	explicit ModuleDirectoryImpl( const std::string& confDir_)
		:m_confDir(confDir_){}
	virtual ~ModuleDirectoryImpl(){}

	/// \brief Add a configured object constructor to the directory.
	bool addObjectConstructor( const ConfiguredObjectConstructor* c);

	/// \brief Add a simple object constructor (constructor for objects without configuration) to the directory.
	bool addObjectConstructor( const SimpleObjectConstructor* c);

	/// \brief Get the object constructor for the configuration section, keyword pair.
	/// \param[in] section	the section (parent) of the configuration
	/// \param[in] keyword	the keyword in the section
	virtual const ConfiguredObjectConstructor* getConfiguredObjectConstructor( const std::string& section, const std::string& keyword ) const;

	/// \brief Get the object constructor for the specified object class name
	virtual const ConfiguredObjectConstructor* getConfiguredObjectConstructor( const std::string& objectClassName ) const;

	/// \brief Get the list of simple object constructor
	virtual const std::vector<const SimpleObjectConstructor*>& getSimpleObjectConstructorList() const;

	/// \brief Get the absolute path of a module
	/// \brief param[in] moduleName name of the module to resolve
	/// \brief param[in] configuredDirectory not expanded name of directory configured as "directory" in the "LoadModules" section of the configuration
	/// \brief param[in] useDefaultModuleDir wheter to load modules from the default system module dir (only test programs may not do so)
	virtual std::string getAbsoluteModulePath( const std::string& moduleName, const std::string& configuredDirectory, bool useDefaultModuleDir=true) const;

	/// \brief Load the list of modules specified by their absolute path
	/// \brief param[in] modFiles list of modules to load into this structure
	bool loadModules( const std::vector<std::string>& modFiles);

private:
	std::string m_confDir;									///< configuration directory needed as base for calculating the absolute path of a module
	std::vector< const ConfiguredObjectConstructor* > m_configuredObjectConstructor;	///< list of configurable object constructors
	std::vector< const SimpleObjectConstructor* >	m_simpleObjectConstructor;		///< list of simple object constructors
};

}}//namespace

#endif
