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
///\file wolfilterCommandLine.hpp
///\brief Interface for the wolfilter call command line
#ifndef _Wolframe_WOLFILTER_COMMANDLINE_HPP_INCLUDED
#define _Wolframe_WOLFILTER_COMMANDLINE_HPP_INCLUDED
#include "libconfig/moduleDirectoryImpl.hpp"
#include "libconfig/procProviderConfiguration.hpp"
#include "libconfig/AAAAproviderConfiguration.hpp"
#include "libconfig/databaseProviderConfiguration.hpp"
#include "types/propertyTree.hpp"
#include "logger/logger-v1.hpp"
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>
#include <iostream>

namespace _Wolframe {
namespace config {

class WolfilterCommandLine
{
public:
	WolfilterCommandLine( int argc, char **argv, const std::string& referencePath, bool useDefaultModuleDir, bool useDefaultConfigIfNotDefined);
	~WolfilterCommandLine()
	{
		delete m_moduleDirectory;
	}

	bool printhelp() const						{return m_printhelp;}
	bool printversion() const					{return m_printversion;}
	const std::string& inputfile() const				{return m_inputfile;}
	const std::vector<std::string>& modules() const			{return m_modules;}
	const std::string& cmd() const					{return m_cmd;}
	const std::string& inputfilter() const				{return m_inputfilter;}
	const std::string& outputfilter() const				{return m_outputfilter;}
	const std::string& protocol() const				{return m_protocol;}
	const config::AAAAproviderConfiguration& aaaaProviderConfig() const {return *m_aaaaProviderConfig;}
	const config::DatabaseProviderConfiguration& dbProviderConfig() const	{return *m_dbProviderConfig;}
	const config::ProcProviderConfiguration& procProviderConfig() const {return *m_procProviderConfig;}
	const module::ModuleDirectory& moduleDirectory() const		{return *m_moduleDirectory;}
	const std::string& configurationPath() const			{return m_configurationPath;}

	static void print( std::ostream &);

private:
	config::ConfigurationTree getProcProviderConfigTree() const;
	config::ConfigurationTree getDBProviderConfigTree( const std::string& dbopt) const;
	std::vector<std::string> configModules( bool useDefaultModuleDir) const;
	config::ConfigurationNode getConfigNode( const std::string& name) const;

private:
	bool m_printhelp;
	bool m_printversion;
	log::LogLevel::Level m_loglevel;
	std::string m_logfile;
	std::string m_inputfile;
	std::vector<std::string> m_modules;
	types::PropertyTree::Node m_dbconfig;
	types::PropertyTree m_config;
	std::string m_cmd;
	std::string m_inputfilter;
	std::string m_outputfilter;
	std::string m_protocol;
	boost::shared_ptr<config::AAAAproviderConfiguration> m_aaaaProviderConfig;
	boost::shared_ptr<config::ProcProviderConfiguration> m_procProviderConfig;
	boost::shared_ptr<config::DatabaseProviderConfiguration> m_dbProviderConfig;
	module::ModuleDirectoryImpl* m_moduleDirectory;
	std::string m_configurationPath;
};

}}//namespace
#endif

