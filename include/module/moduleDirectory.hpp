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
/// \file module/moduleDirectory.hpp
/// \brief Module objects directory interface

#ifndef _MODULE_DIRECTORY_INTERFACE_HPP_INCLUDED
#define _MODULE_DIRECTORY_INTERFACE_HPP_INCLUDED

#include <string>
#include <list>
#include "module/moduleInterface.hpp"

namespace _Wolframe {
namespace module {

/// \class ModulesDirectory
/// \brief Interface to the modules directory used by the constructors of the providers to build themselves.
class ModulesDirectory
{
public:
	virtual ~ModulesDirectory(){}
	/// \brief Get the configured builder for the configuration section, keyword pair.
	/// \param[in] section	the section (parent) of the configuration
	/// \param[in] keyword	the keyword in the section
	virtual const ConfiguredBuilder* getConfiguredBuilder( const std::string& section, const std::string& keyword ) const=0;

	/// \brief Get the configured builder for the specified object class name
	virtual const ConfiguredBuilder* getConfiguredBuilder( const std::string& objectClassName ) const=0;

	/// \brief Get the list of simple builders
	virtual const std::vector<const SimpleBuilder*>& getSimpleBuilderList() const=0;

	/// \brief Get the absolute path of a module
	/// \brief param[in] moduleName name of the module to resolve
	/// \brief param[in] configuredDirectory not expanded name of directory configured as "directory" in the "LoadModules" section of the configuration
	/// \brief param[in] useDefaultModuleDir wheter to load modules from the default system module dir (only test programs may not do so)
	virtual std::string getAbsoluteModulePath( const std::string& moduleName, const std::string& configuredDirectory, bool useDefaultModuleDir=true) const=0;
};

}}//namespace
#endif
