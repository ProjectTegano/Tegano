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
///\file wolfwizardCommandLine.hpp
///\brief Interface for the wolfwizard call command line
#ifndef _Wolframe_WOLFWIZARD_COMMANDLINE_HPP_INCLUDED
#define _Wolframe_WOLFWIZARD_COMMANDLINE_HPP_INCLUDED
#include "libconfig/moduleDirectoryImpl.hpp"
#include "libprovider/procProviderImpl.hpp"
#include "types/propertyTree.hpp"
#include <string>
#include <vector>
#include <iostream>

namespace _Wolframe {
namespace config {

class WolfwizardCommandLine
{
public:
	WolfwizardCommandLine( int argc, char **argv, const std::string& referencePath_);
	~WolfwizardCommandLine(){}

	bool printhelp() const						{return m_printhelp;}
	bool printversion() const					{return m_printversion;}
	const std::string& configfile() const				{return m_configfile;}

	static void print( std::ostream& out);
	const config::ConfigurationNode& providerconfig() const		{return m_providerconfig;}
	const std::string& configurationPath() const			{return m_configurationPath;}
	const module::ModuleDirectory& moduleDirectory() const		{return *m_moduleDirectory;}

private:
	std::vector<std::string> configModules() const;
	config::ConfigurationNode getConfigNode( const std::string& name) const;

private:
	bool m_printhelp;
	bool m_printversion;
	std::string m_configfile;
	config::ConfigurationTree m_config;
	config::ConfigurationNode m_providerconfig;
	module::ModuleDirectoryImpl* m_moduleDirectory;
	std::string m_configurationPath;
	std::vector<std::string> m_modules;
};

}}//namespace
#endif

