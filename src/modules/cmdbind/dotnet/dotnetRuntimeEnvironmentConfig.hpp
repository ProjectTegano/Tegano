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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file dotnetRuntimeEnvironmentConfig.hpp
///\brief Interface .NET runtime environment configuration
#ifndef _Wolframe_DOTNET_RUNTIME_ENVIRONMENT_CONFIGURATION_HPP_INCLUDED
#define _Wolframe_DOTNET_RUNTIME_ENVIRONMENT_CONFIGURATION_HPP_INCLUDED
#include "config/configurationTree.hpp"
#include "config/configurationObject.hpp"
#include "processor/procProviderInterface.hpp"
#include "processor/execContext.hpp"
#include <string>
#include <map>

namespace _Wolframe {
namespace module {

///\brief Named configuration definition
class DotnetRuntimeEnvironmentConfig
	:public config::ConfigurationObject
{
public:
	class AssemblyDescription
	{
	public:
		std::string name;
		std::string description;

		AssemblyDescription(){}
		AssemblyDescription( const std::string& description_);
		AssemblyDescription( const AssemblyDescription& o)
			:name(o.name),description(o.description){}
	};

public:
	DotnetRuntimeEnvironmentConfig( const std::string& className_, const std::string& configSection_, const std::string& configKeyword_)
		:config::ConfigurationObject( className_, configSection_, configKeyword_)
	{}
	virtual ~DotnetRuntimeEnvironmentConfig(){}

	///\brief Parse the configuration
	///\param[in] pt configuration tree
	///\param[in] modules module directory
	virtual bool parse( const config::ConfigurationNode& pt, const std::string&, const ModuleDirectory* modules);

	///\brief Set canonical path for files referenced as relative path in configuration
	///\param[in] referencePath reference path
	virtual void setCanonicalPathes( const std::string& referencePath);

	virtual bool check() const;

	virtual bool checkReferences( const proc::ProcessorProviderInterface*) const {return true;}

	virtual void print( std::ostream& os, size_t indent ) const;

	const std::vector<AssemblyDescription>& assemblylist() const
	{
		return m_assemblylist;
	}

	const std::string& clrversion() const
	{
		return m_clrversion;
	}

	const std::string& typelibpath() const
	{
		return m_typelibpath;
	}

private:
	std::string m_clrversion;
	std::string m_typelibpath;

	std::vector<AssemblyDescription> m_assemblylist;
	config::ConfigurationTree::Position m_config_pos;
};

}} //namespace
#endif

