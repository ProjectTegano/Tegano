/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
#include "langbind/appObjects.hpp"
#include "database/DBprovider.hpp"
#include "filter/ptreefilter.hpp"
#include "filter/tostringfilter.hpp"
#include "moduleInterface.hpp"
#include "config/ConfigurationTree.hpp"
#include "serialize/structOptionParser.hpp"
#include "utils/miscUtils.hpp"
#include "utils/doctype.hpp"
#include "config/structSerialize.hpp"
#include "logger-v1.hpp"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <cstring>
#include <algorithm>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

using namespace _Wolframe;
using namespace _Wolframe::config;

static std::string configurationTree_tostring( const boost::property_tree::ptree& pt)
{
	langbind::TypedInputFilterR inp( new langbind::PropertyTreeInputFilter( pt));
	langbind::ToStringFilter* res;
	langbind::TypedOutputFilterR out( res = new langbind::ToStringFilter( "\t"));
	langbind::RedirectFilterClosure redirect( inp, out);
	if (!redirect.call()) throw std::runtime_error( "can't map configuration tree to string");
	return res->content();
}

static boost::property_tree::ptree getTreeNode( const boost::property_tree::ptree& tree, const std::string& name)
{
	bool found = false;
	boost::property_tree::ptree rt;
	boost::property_tree::ptree::const_iterator gi = tree.begin(), ge = tree.end();
	for (; gi != ge; ++gi)
	{
		if (boost::algorithm::iequals( gi->first, name))
		{
			if (found) throw std::runtime_error( std::string("duplicate '") + name + "' node in configuration");
			rt = gi->second;
			found = true;
		}
	}
	return rt;
}

boost::property_tree::ptree WolfilterCommandLine::getConfigNode( const std::string& name) const
{
	return getTreeNode( m_config, name);
}

std::vector<std::string> WolfilterCommandLine::configModules() const
{
	std::vector<std::string> rt;
	boost::property_tree::ptree module_section = getConfigNode( "LoadModules");
	boost::property_tree::ptree::const_iterator mi = module_section.begin(), me = module_section.end();
	for (; mi != me; ++mi)
	{
		if (boost::algorithm::iequals( mi->first, "module"))
		{
			rt.push_back( utils::getCanonicalPath( mi->second.get_value<std::string>(), m_referencePath));
		}
	}
	return rt;
}

config::ConfigurationTree WolfilterCommandLine::getDBProviderConfigTree( const std::string& dbopt) const
{
	boost::property_tree::ptree dbopt_tree = serialize::structOptionTree( dbopt);
	boost::property_tree::ptree rt;

	std::vector<std::pair<std::string,std::string> >
		dbhl = m_modulesDirectory.getConfigurableSectionKeywords( ObjectConstructorBase::DATABASE_OBJECT);
	if (dbhl.size() > 1)
	{
		// only one database module allowed:
		LOG_WARNING << "More than one database module loaded for wolfiler. Assuming that the first one (" << dbhl.begin()->first << "/" << dbhl.begin()->second << ") is the one to use";
	}
	else if (dbhl.empty())
	{
		throw std::runtime_error( "no database module loaded for wolfilter but database configured");
	}
	rt.add_child( dbhl.begin()->second, dbopt_tree);
	return rt;
}

config::ConfigurationTree WolfilterCommandLine::getProcProviderConfigTree() const
{
	boost::property_tree::ptree proccfg;
	try
	{
		if (!m_dbconfig.empty())
		{
			boost::property_tree::ptree::const_iterator pi = m_dbconfig.begin(), pe = m_dbconfig.begin();
			std::string dbLabel = pi->second.get<std::string>( "identifier");
			if (++pi == pe)
			{
				throw std::runtime_error( "two databases (--database) specified on command line");
			}
			if (dbLabel.empty())
			{
				throw std::runtime_error( "database configuration without 'identifier' field");
			}
			proccfg.add_child( "database", boost::property_tree::ptree( dbLabel));
		}
		std::vector<std::pair<std::string,std::string> >
			cmdhl = m_modulesDirectory.getConfigurableSectionKeywords( ObjectConstructorBase::CMD_HANDLER_OBJECT);

		std::map<std::string,std::vector<std::string> > cmdhandler_programs;
		std::vector<std::string>::const_iterator gi = m_programs.begin(), ge = m_programs.end();
		for (; gi != ge; ++gi)
		{
			bool cfgid_set = false;
			std::pair<std::string,std::string> cfgid;
			std::string extension = utils::getFileExtension( *gi);
			if (!extension.empty())
			{
				std::string cmdhndname = std::string( extension.c_str() +1);
				std::vector<std::pair<std::string,std::string> >::const_iterator pi = cmdhl.begin(), pe = cmdhl.end();
				for (; pi != pe; ++pi)
				{
					if (boost::iequals( pi->first, "cmdhandler")
					&&  boost::iequals( pi->second, cmdhndname))
					{
						if (cfgid_set)
						{
							throw std::runtime_error( std::string( "more than one command handler module loaded that match to program selected (") + cmdhndname + ")");
						}
						cfgid = *pi;
						cfgid_set = true;
					}
				}
				if (cfgid_set)
				{
					cmdhandler_programs[ cmdhndname].push_back( *gi);
				}
			}
			if (!cfgid_set)
			{
				proccfg.add_child( "program", boost::property_tree::ptree( *gi));
			}
		}
		std::map<std::string,std::vector<std::string> >::const_iterator mi = cmdhandler_programs.begin(), me = cmdhandler_programs.end();
		for (; mi != me; ++mi)
		{
			boost::property_tree::ptree programcfg,cmdhlcfg;
			std::vector<std::string>::const_iterator ri = mi->second.begin(), re = mi->second.end();
			for (; ri != re; ++ri)
			{
				programcfg.add_child( "program", boost::property_tree::ptree(*ri));
			}
			cmdhlcfg.add_child( mi->first, programcfg);
			proccfg.add_child( "cmdhandler", cmdhlcfg);
		}
	}
	catch (std::exception& e)
	{
		throw std::runtime_error( std::string( "could not build wolframe configuration from given options: ") + e.what());
	}
	return proccfg;
}

namespace po = boost::program_options;

struct OptionStruct
{
	po::options_description fopt;
	po::positional_options_description popt;

	OptionStruct()
		:fopt("Options")
	{
		fopt.add_options()
			( "version,v", "print version" )
			( "help,h", "print help message" )
			( "loglevel,l", po::value<std::string>(), "specify the log level on console" )
			( "input,f", po::value<std::string>(), "specify input file to process by path" )
			( "input-filter,i", po::value<std::string>(), "specify input filter by name" )
			( "output-filter,o", po::value<std::string>(), "specify output filter by name" )
			( "module,m", po::value< std::vector<std::string> >(), "specify module to load by path" )
			( "program,p", po::value< std::vector<std::string> >(), "specify program to load by path" )
			( "config,C", po::value<std::string>(), "specify configuration file to load" )
			( "database,D", po::value<std::string>(), "specifiy transaction database" )
			( "cmd", po::value<std::string>(), "name of the command to execute")
			;

		popt.add( "cmd", 1);
	}
};

WolfilterCommandLine::WolfilterCommandLine( int argc, char** argv, const std::string& referencePath)
	:m_printhelp(false)
	,m_printversion(false)
	,m_inbufsize(8<<10)
	,m_outbufsize(8<<10)
	,m_referencePath(referencePath)
{
	static const OptionStruct ost;
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
	if (vmap.count( "loglevel"))
	{
		m_loglevel = vmap["loglevel"].as<std::string>();
		_Wolframe::log::LogBackend::instance().setConsoleLevel( log::LogLevel::strToLogLevel( m_loglevel));
	}
	if (vmap.count( "config"))
	{
		std::string configfile = utils::getCanonicalPath( vmap["config"].as<std::string>(), referencePath);
		m_config = utils::readPropertyTreeFile( configfile);
		if (vmap.count( "module")) throw std::runtime_error( "incompatible options: --config specified with --module");
		if (vmap.count( "program")) throw std::runtime_error( "incompatible options: --config specified with --program");
		if (vmap.count( "database")) throw std::runtime_error( "incompatible options: --config specified with --database");
	}
	if (vmap.count( "input")) m_inputfile = vmap["input"].as<std::string>();
	if (vmap.count( "module"))
	{
		m_modules = vmap["module"].as<std::vector<std::string> >();
		std::vector<std::string>::iterator itr=m_modules.begin(), end=m_modules.end();
		for (; itr != end; ++itr)
		{
			*itr = utils::getCanonicalPath( *itr, referencePath);
		}
	}
	// Load modules
	std::vector<std::string> cfgmod = configModules();
	std::copy( cfgmod.begin(), cfgmod.end(), std::back_inserter( m_modules));
	std::list<std::string> modfiles;
	std::copy( m_modules.begin(), m_modules.end(), std::back_inserter(modfiles));
	if (!LoadModules( m_modulesDirectory, modfiles))
	{
		throw std::runtime_error( "Modules could not be loaded");
	}
	if (vmap.count( "program"))
	{
		m_programs = vmap["program"].as<std::vector<std::string> >();
	}
	if (vmap.count( "cmd")) m_cmd = vmap["cmd"].as<std::string>();
	if (vmap.count( "input-filter")) m_inputfilter = vmap["input-filter"].as<std::string>();
	if (vmap.count( "output-filter")) m_outputfilter = vmap["output-filter"].as<std::string>();
	if (vmap.count( "database"))
	{
		m_dbconfig = getDBProviderConfigTree( vmap["database"].as<std::string>());
	}
	else
	{
		m_dbconfig = getConfigNode( "database");
	}
	if (m_outputfilter.empty() && !m_inputfilter.empty()) m_outputfilter = m_inputfilter; //... default same filter for input and output
	if (m_inputfilter.empty() && !m_outputfilter.empty()) m_inputfilter = m_outputfilter; //... default same filter for input and output

	const char* bp;
	bp = std::strchr( m_inputfilter.c_str(), '/');
	if (bp)
	{
		m_inbufsize = (std::size_t)atoi( bp+1);
		m_inputfilter.resize( bp-m_inputfilter.c_str());
	}
	bp = std::strchr( m_outputfilter.c_str(), '/');
	if (bp)
	{
		m_outbufsize = (std::size_t)atoi( bp+1);
		m_outputfilter.resize( bp-m_outputfilter.c_str());
	}

	std::ostringstream dd;
	dd << ost.fopt;
	m_helpstring = dd.str();

	// Load, instantiate and check the configuration:
	m_dbProviderConfig.reset( new db::DBproviderConfig());

	LOG_DEBUG << "Created database provider configuration from command line:";
	LOG_DEBUG << configurationTree_tostring( m_dbconfig);

	if (!m_dbProviderConfig->parse( m_dbconfig, "", &m_modulesDirectory))
	{
		throw std::runtime_error( "Database provider configuration could not be created from command line");
	}
	m_dbProviderConfig->setCanonicalPathes( referencePath);
	if (!m_dbProviderConfig->check())
	{
		throw std::runtime_error( "error in command line. failed to setup a valid database provider configuration");
	}

	m_procProviderConfig.reset( new proc::ProcProviderConfig());
	boost::property_tree::ptree ppcfg;
	if (vmap.count( "config"))
	{
		ppcfg = getConfigNode( "Processor");
	}
	else
	{
		ppcfg = getProcProviderConfigTree();

		LOG_DEBUG << "Created processor provider configuration from command line:";
		LOG_DEBUG << configurationTree_tostring( ppcfg);
	}
	if (!m_procProviderConfig->parse( ppcfg, "", &m_modulesDirectory))
	{
		throw std::runtime_error( "Processor provider configuration could not be created from command line");
	}
	m_procProviderConfig->setCanonicalPathes( referencePath);
	if (!m_procProviderConfig->check())
	{
		throw std::runtime_error( "error in command line. failed to setup a valid processor provider configuration");
	}
}

void WolfilterCommandLine::print(std::ostream& out) const
{
	out << "Call:" << std::endl;
	out << "\twolfilter [OPTION] <cmd> <inputfilter> <outputfilter>" << std::endl;
	out << m_helpstring << std::endl;
}



