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
///\file wolfilterCommandLine.cpp
///\brief Implementation of the options of a wolfilter call
#include "wolfilterCommandLine.hpp"
#include "filter/ptreefilter.hpp"
#include "config/configurationTree.hpp"
#include "types/propertyTree.hpp"
#include "serialize/structOptionParser.hpp"
#include "serialize/configSerialize.hpp"
#include "utils/fileUtils.hpp"
#include "utils/stringUtils.hpp"
#include "filter/redirectFilterClosure.hpp"
#include "logger/logger-v1.hpp"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <cstring>
#include <algorithm>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

using namespace _Wolframe;
using namespace _Wolframe::config;

//TODO: NOT TO DEFINE HERE (it is here because appProperties.cpp is not in a src/libwolframed.a -> Issue #95)
static const char* defaultMainConfig()		{
#ifdef DEFAULT_MAIN_CONFIGURATION_FILE
		#define DO_STRINGIFY(x)	#x
		return DO_STRINGIFY( DEFAULT_MAIN_CONFIGURATION_FILE );
#else
		return "/etc/wolframe/wolframe.conf";
#endif
	}
static const char* defaultUserConfig()		{ return "~/wolframe.conf"; }
static const char* defaultLocalConfig()		{ return "./wolframe.conf"; }
static const char* getDefaultConfigFile()
{
	if (utils::fileExists( defaultMainConfig())) return defaultMainConfig();
	if (utils::fileExists( defaultUserConfig())) return defaultUserConfig();
	if (utils::fileExists( defaultLocalConfig())) return defaultLocalConfig();
	return 0;
}

static std::string configurationTree_tostring( const types::PropertyTree::Node& pt)
{
	langbind::TypedInputFilterR inp( new langbind::PropertyTreeInputFilter( pt));
	return utils::filterToString( *inp, utils::ptreePrintFormat());
}

config::ConfigurationNode WolfilterCommandLine::getConfigNode( const std::string& name) const
{
	return m_config.root().getChildrenJoined( name);
}

std::vector<std::string> WolfilterCommandLine::configModules( bool useDefaultModuleDir) const
{
	std::vector<std::string> rt;
	config::ConfigurationNode module_section = getConfigNode( "LoadModules");
	std::string directory;

	types::PropertyTree::Node::const_iterator mi = module_section.begin(), me = module_section.end();
	for (; mi != me; ++mi)
	{
		if (boost::algorithm::iequals( mi->first, "directory"))
		{
			if (!directory.empty()) throw std::runtime_error( "duplicate definition of 'directory' in section LoadModules");
			directory = mi->second.data();
			if (directory.empty()) throw std::runtime_error( "empty definition of 'directory' in section LoadModules");
		}
	}
	mi = module_section.begin();
	for (; mi != me; ++mi)
	{
		if (boost::algorithm::iequals( mi->first, "module"))
		{
			std::string absmodpath = m_moduleDirectory->getAbsoluteModulePath( mi->second.data(), directory, useDefaultModuleDir);
			if (absmodpath.empty())
			{
				throw std::runtime_error( std::string("could not resolve configured module path '") + mi->second.data() + "'");
			}
			rt.push_back( absmodpath);
		}
	}
	return rt;
}

namespace po = boost::program_options;

struct WolfilterOptionStruct
{
	po::options_description fopt;
	po::positional_options_description popt;

	WolfilterOptionStruct()
		:fopt("Options")
	{
		fopt.add_options()
			( "version,v", "print version" )
			( "help,h", "print help message" )
			( "loglevel,l", po::value<std::string>(), "specify the log level on console" )
			( "logfile", po::value<std::string>(), "specify a file for the log output" )
#if defined(_WIN32) // DISABLED because Windows cannot handle boost::program_options::value multitoken this way
			( "verbosity,t", po::value< std::vector<std::string> >()->multitoken()->zero_tokens(), "variant of option --loglevel: Raise verbosity level with (-t,-tt,-ttt,..)" )
#endif
			( "input,f", po::value<std::string>(), "specify input file to process by path" )
			( "input-filter,i", po::value<std::string>(), "specify input filter by name" )
			( "output-filter,o", po::value<std::string>(), "specify output filter by name" )
			( "filter,e", po::value<std::string>(), "specify input/output filter by name (if not specified separately)" )
			( "module,m", po::value< std::vector<std::string> >(), "specify module to load by path" )
			( "config,c", po::value<std::string>(), "specify configuration file to load" )
			( "protocol,p", po::value<std::string>(), "specify protocol to use (default no protocol)" )
			( "cmd", po::value<std::string>(), "name of the command to execute")
			;

		popt.add( "cmd", 1);
	}
};

WolfilterCommandLine::WolfilterCommandLine( int argc, char** argv, const std::string& referencePath, bool useDefaultModuleDir, bool useDefaultConfigIfNotDefined)
	:m_printhelp(false)
	,m_printversion(false)
	,m_loglevel(_Wolframe::log::LogLevel::LOGLEVEL_WARNING)
	,m_moduleDirectory(0)
	,m_configurationPath(referencePath)
{
	static const WolfilterOptionStruct ost;
	po::variables_map vmap;
	try
	{
		po::store( po::command_line_parser(argc, argv).options(ost.fopt).positional(ost.popt).run(), vmap);
		po::notify( vmap);
	}
	catch (std::exception& e)
	{
		throw std::runtime_error( std::string("error parsing command line options: ") + e.what());
	}
	m_printversion = vmap.count( "version");
	m_printhelp = vmap.count( "help");
	int tracelevel = vmap.count( "verbosity");
	bool loglevel_set = vmap.count( "loglevel");
	if (loglevel_set && tracelevel)
	{
		throw std::runtime_error( "incompatible options: --loglevel (-l) specified with --verbosity (-t)");
	}
	if (loglevel_set)
	{
		m_loglevel = log::LogLevel::strToLogLevel( vmap["loglevel"].as<std::string>());
		_Wolframe::log::LogBackend::instance().setConsoleLevel( m_loglevel);
	}
	if (tracelevel)
	{
		if (tracelevel >= 1) m_loglevel = _Wolframe::log::LogLevel::LOGLEVEL_INFO;
		if (tracelevel >= 2) m_loglevel = _Wolframe::log::LogLevel::LOGLEVEL_DEBUG;
		if (tracelevel >= 3) m_loglevel = _Wolframe::log::LogLevel::LOGLEVEL_TRACE;
		if (tracelevel >= 4) m_loglevel = _Wolframe::log::LogLevel::LOGLEVEL_DATA;
		if (tracelevel >= 5) m_loglevel = _Wolframe::log::LogLevel::LOGLEVEL_DATA2;
		_Wolframe::log::LogBackend::instance().setConsoleLevel( m_loglevel);
	}
	if (vmap.count( "logfile"))
	{
		m_logfile = vmap["logfile"].as<std::string>();
		_Wolframe::log::LogBackend::instance().setLogfileName( m_logfile);
		_Wolframe::log::LogBackend::instance().setLogfileLevel( m_loglevel);
	}
	bool hasConfig = false;
	std::string configfile;
	if (vmap.count( "config"))
	{
		configfile = vmap["config"].as<std::string>();
		if (m_configurationPath.size() != 0)
		{
			configfile = utils::getCanonicalPath( configfile, m_configurationPath);
		}
		hasConfig = true;
	}
	else
	{
#if !defined(_WIN32)
		const char* defaultConfigFile = getDefaultConfigFile();
		if (defaultConfigFile && useDefaultConfigIfNotDefined)
		{
			LOG_DEBUG << "No configuration file specified on command line. Using default configuration file '" << defaultConfigFile << "'";
			configfile = defaultConfigFile;
			hasConfig = true;
		}
#else
		if (useDefaultConfigIfNotDefined)
		{
			LOG_TRACE << "No configuration file specified on command line";
		}
#endif
	}
	if (hasConfig)
	{
		m_configurationPath = boost::filesystem::path( configfile ).branch_path().string();
		m_config = utils::readPropertyTreeFile( configfile);
	}
	else
	{
		LOG_DEBUG << "Running without configuration";
	}
	m_moduleDirectory = new module::ModuleDirectoryImpl( m_configurationPath);

	if (vmap.count( "input"))
	{
		m_inputfile = vmap["input"].as<std::string>();
	}
	if (vmap.count( "protocol"))
	{
		m_protocol = vmap["protocol"].as<std::string>();
	}
	if (hasConfig)
	{
		std::vector<std::string> cfgmod = configModules( useDefaultModuleDir);
		std::copy( cfgmod.begin(), cfgmod.end(), std::back_inserter( m_modules));
	}
	if (vmap.count( "module"))
	{
		std::vector<std::string> mods = vmap["module"].as<std::vector<std::string> >();
		std::vector<std::string>::iterator mi=mods.begin(), me=mods.end();
		for (; mi != me; ++mi)
		{
			std::string absmodpath = m_moduleDirectory->getAbsoluteModulePath( *mi, m_configurationPath, useDefaultModuleDir);
			if (absmodpath.empty())
			{
				throw std::runtime_error( std::string("could not resolve command line module path '") + *mi + "'");
			}
			m_modules.push_back( absmodpath);
			LOG_DEBUG << "Defined additional module to load '" << absmodpath << "'";
		}
	}
	std::vector<std::string> modfiles;
	std::copy( m_modules.begin(), m_modules.end(), std::back_inserter( modfiles));

	if (!m_moduleDirectory->loadModules( modfiles))
	{
		throw std::runtime_error( "Modules could not be loaded");
	}
	if (vmap.count( "cmd")) m_cmd = vmap["cmd"].as<std::string>();
	if (vmap.count( "filter"))
	{
		m_inputfilter = vmap["filter"].as<std::string>();
		m_outputfilter = vmap["filter"].as<std::string>();
		if (vmap.count( "input-filter")) throw std::runtime_error( "incompatible options: --filter specified with --input-filter");
		if (vmap.count( "output-filter")) throw std::runtime_error( "incompatible options: --filter specified with --output-filter");
	}
	else if (vmap.count( "input-filter") || vmap.count( "output-filter"))
	{
		if (vmap.count( "input-filter")) m_inputfilter = vmap["input-filter"].as<std::string>();
		if (vmap.count( "output-filter")) m_outputfilter = vmap["output-filter"].as<std::string>();
	}
	m_dbconfig = getConfigNode( "database");

	// Load, instantiate and check the configuration:
	m_dbProviderConfig.reset( new config::DatabaseProviderConfiguration());

	LOG_DEBUG << "Database provider configuration:";
	LOG_DEBUG << configurationTree_tostring( m_dbconfig);

	if (!m_dbProviderConfig->parse( m_dbconfig, "", m_moduleDirectory))
	{
		throw std::runtime_error( "database provider configuration could not be parsed");
	}
	m_dbProviderConfig->setCanonicalPathes( m_configurationPath);
	if (!m_dbProviderConfig->check())
	{
		throw std::runtime_error( "error in database provider configuration");
	}

	m_procProviderConfig.reset( new config::ProcProviderConfiguration());
	types::PropertyTree::Node ppcfg;
	if (hasConfig)
	{
		ppcfg = getConfigNode( "Processor");
	}
	if (!m_procProviderConfig->parse( ppcfg, "", m_moduleDirectory))
	{
		throw std::runtime_error( "processor provider configuration could not be parsed");
	}
	m_procProviderConfig->setCanonicalPathes( m_configurationPath);
	if (!m_procProviderConfig->check())
	{
		throw std::runtime_error( "error in processor provider configuration");
	}

	m_aaaaProviderConfig.reset( new config::AaaaProviderConfiguration());
	types::PropertyTree::Node aacfg;
	if (hasConfig)
	{
		aacfg = getConfigNode( "AAAA");
	}
	if (!m_aaaaProviderConfig->parse( aacfg, "", m_moduleDirectory))
	{
		throw std::runtime_error( "AAAA provider configuration could not be parsed");
	}
	m_aaaaProviderConfig->setCanonicalPathes( m_configurationPath);
	if (!m_aaaaProviderConfig->check())
	{
		throw std::runtime_error( "error in AAAA provider configuration");
	}
}

void WolfilterCommandLine::print( std::ostream& out)
{
	static const WolfilterOptionStruct ost;
	out << "Call:" << std::endl;
	out << "\twolfilter [OPTION] <cmd>" << std::endl;
	out << ost.fopt << std::endl;
}



