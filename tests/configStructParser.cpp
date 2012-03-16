/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
/// \file configStructParser.cpp
/// \brief test for configuration parser with wolframe example configuration

#include "config/description.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "miscUtils.hpp"
#ifdef _WIN32
#pragma warning(disable:4996)
#pragma warning(disable:4127)
#endif
#include <iostream>
#include <boost/property_tree/info_parser.hpp>
#include "logger-v1.hpp"

using namespace _Wolframe;
using namespace config;
using namespace _Wolframe::log;

struct LoggerConfig
{
	struct ToStderr
	{
		LogLevel::Level loglevel;

		ToStderr()
			:loglevel(LogLevel::LOGLEVEL_INFO){}

		static const config::DescriptionBase* description();
	};

	struct ToFile
	{
		LogLevel::Level loglevel;
		std::string filename;

		ToFile()
			:loglevel(LogLevel::LOGLEVEL_INFO){}

		static const config::DescriptionBase* description();
	};

	struct ToSyslog
	{
		LogLevel::Level loglevel;
		SyslogFacility::Facility facility;
		std::string ident;

		ToSyslog()
			:loglevel(LogLevel::LOGLEVEL_INFO)
			,facility(SyslogFacility::WOLFRAME_SYSLOG_FACILITY_LOCAL2){}

		static const config::DescriptionBase* description();
	};

	struct ToEventlog
	{
		LogLevel::Level loglevel;
		std::string name;
		std::string source;

		ToEventlog()
			:loglevel(LogLevel::LOGLEVEL_INFO){}

		static const config::DescriptionBase* description();
	};

	ToEventlog eventlog;
	ToSyslog syslog;
	ToFile filelog;
	ToStderr _stderr;

	static const config::DescriptionBase* description();
};

struct NetConfig
{
	unsigned short threads;
	unsigned short maxConnections;

	struct Endpoint
	{
		std::string name;
		std::string address;
		unsigned short port;
		unsigned short maxConnections;

		Endpoint()
			:port(0),maxConnections(0){}

		static const config::DescriptionBase* description();
	};
	std::vector<Endpoint> endpoints;

	struct SSLEndpoint
	{
		std::string name;
		std::string address;
		unsigned short port;
		unsigned short maxConnections;
		std::string key;
		std::string certificate;
		std::string CAdirectory;
		std::string CAchainFile;
		bool verify;

		SSLEndpoint()
			:port(0),maxConnections(0),verify(true){}

		static const config::DescriptionBase* description();
	};
	std::vector<SSLEndpoint> SSLendpoints;

	NetConfig() :threads(0),maxConnections(0) {}
	static const config::DescriptionBase* description();
};

struct DatabaseConfig
{
	std::string identifier;
	std::string host;
	unsigned short port;
	std::string name;
	std::string user;
	std::string password;
	unsigned short connections;
	unsigned short acquireTimeout;

	DatabaseConfig() :port(0),connections(0),acquireTimeout(0) {}
	static const config::DescriptionBase* description();
};

struct ServiceConfig
{
	std::string name;
	std::string displayName;
	std::string descr;

	static const config::DescriptionBase* description();
};

struct DaemonConfig
{
	std::string user;
	std::string group;
	std::string pidFile;

	static const config::DescriptionBase* description();
};

struct AAAAConfig
{
	struct AuthConfig
	{
		std::string file;
		std::string database;

		static const config::DescriptionBase* description();
	};
	struct AuditConfig
	{
		std::string file;

		static const config::DescriptionBase* description();
	};
};


struct Configuration
{
	ServiceConfig service;
	DaemonConfig daemon;
	NetConfig net;
	LoggerConfig logger;
	std::vector<DatabaseConfig> database;

	static const config::DescriptionBase* description();
};


const config::DescriptionBase* LoggerConfig::ToStderr::description()
{
	struct ThisDescription :public config::Description<LoggerConfig::ToStderr>
	{
		ThisDescription()
		{
			(*this)
			( "level",		&LoggerConfig::ToStderr::loglevel);
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* LoggerConfig::ToFile::description()
{
	struct ThisDescription :public config::Description<LoggerConfig::ToFile>
	{
		ThisDescription()
		{
			(*this)
			( "level",		&LoggerConfig::ToFile::loglevel)
			( "filename",		&LoggerConfig::ToFile::filename);
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* LoggerConfig::ToSyslog::description()
{
	struct ThisDescription :public config::Description<LoggerConfig::ToSyslog>
	{
		ThisDescription()
		{
			(*this)
			( "level",		&LoggerConfig::ToSyslog::loglevel)
			( "facility",		&LoggerConfig::ToSyslog::facility)
			( "ident",		&LoggerConfig::ToSyslog::ident);
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* LoggerConfig::ToEventlog::description()
{
	struct ThisDescription :public config::Description<LoggerConfig::ToEventlog>
	{
		ThisDescription()
		{
			(*this)
			( "level",		&LoggerConfig::ToEventlog::loglevel)
			( "name",		&LoggerConfig::ToEventlog::name)
			( "source",		&LoggerConfig::ToEventlog::source);
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* LoggerConfig::description()
{
	struct ThisDescription :public config::Description<LoggerConfig>
	{
		ThisDescription()
		{
			(*this)
			( "eventlog",		&LoggerConfig::eventlog)
			( "syslog",		&LoggerConfig::syslog)
			( "logfile",		&LoggerConfig::filelog)
			( "stderr",		&LoggerConfig::_stderr);
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* NetConfig::description()
{
	struct ThisDescription :public config::Description<NetConfig>
	{
		ThisDescription()
		{
			(*this)
			( "threads",		&NetConfig::threads)
			( "maxConnections",	&NetConfig::maxConnections)
			( "endpoint",		&NetConfig::endpoints)
			( "SSLendpoint",	&NetConfig::SSLendpoints);
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* NetConfig::Endpoint::description()
{
	struct ThisDescription :public config::Description<NetConfig::Endpoint>
	{
		ThisDescription()
		{
			(*this)
			( "name",		&NetConfig::Endpoint::name)
			( "address",		&NetConfig::Endpoint::address)
			( "port",		&NetConfig::Endpoint::port)
			( "maxConnections",	&NetConfig::Endpoint::maxConnections);
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* NetConfig::SSLEndpoint::description()
{
	struct ThisDescription :public config::Description<NetConfig::SSLEndpoint>
	{
		ThisDescription()
		{
			(*this)
			( "name",		&NetConfig::SSLEndpoint::name)
			( "address",		&NetConfig::SSLEndpoint::address)
			( "port",		&NetConfig::SSLEndpoint::port)
			( "maxConnections",	&NetConfig::SSLEndpoint::maxConnections)
			( "key",		&NetConfig::SSLEndpoint::key)
			( "certificate",	&NetConfig::SSLEndpoint::certificate)
			( "CAdirectory",	&NetConfig::SSLEndpoint::CAdirectory)
			( "CAchainFile",	&NetConfig::SSLEndpoint::CAchainFile)
			( "verify",		&NetConfig::SSLEndpoint::verify);
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* DatabaseConfig::description()
{
	struct ThisDescription :public config::Description<DatabaseConfig>
	{
		ThisDescription()
		{
			(*this)
			( "identifier",		&DatabaseConfig::identifier)
			( "host",		&DatabaseConfig::host)
			( "port",		&DatabaseConfig::port)
			( "name",		&DatabaseConfig::name)
			( "user",		&DatabaseConfig::user)
			( "password",		&DatabaseConfig::password)
			( "connections",	&DatabaseConfig::connections)
			( "acquireTimeout",	&DatabaseConfig::acquireTimeout);
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* ServiceConfig::description()
{
	struct ThisDescription :public config::Description<ServiceConfig>
	{
		ThisDescription()
		{
			(*this)
			( "servicename",	&ServiceConfig::name)
			( "displayName",	&ServiceConfig::displayName)
			( "description",	&ServiceConfig::descr);
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* DaemonConfig::description()
{
	struct ThisDescription :public config::Description<DaemonConfig>
	{
		ThisDescription()
		{
			(*this)
			( "user",		&DaemonConfig::user)
			( "group",		&DaemonConfig::group)
			( "pidFile",		&DaemonConfig::pidFile);
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* Configuration::description()
{
	struct ThisDescription :public config::Description<Configuration>
	{
		ThisDescription()
		{
			(*this)
			( "service",		&Configuration::service)
			( "daemon",		&Configuration::daemon)
			( "logging",		&Configuration::logger)
			( "net",		&Configuration::net);
		}
	};
	static const ThisDescription rt;
	return &rt;
}


int main( int argc, const char** argv)
{
	Configuration cfg;

	if (argc <= 1)
	{
		std::cerr << "missing argument configuration file" << std::endl;
	}
	std::string filename( argv[1]);

	std::string configfile = boost::filesystem::absolute( filename).string();
	if ( !boost::filesystem::exists( configfile))	{
		std::cerr << "Configuration file " << configfile << " does not exist." << std::endl;
		return false;
	}

	std::string errmsg;
	boost::property_tree::ptree pt;
	try
	{
		boost::property_tree::read_info( configfile, pt);
		if (cfg.description()->parse( &cfg, pt, errmsg))
		{
			cfg.description()->print( std::cout, &cfg);
		}
	}
	catch (std::exception& e)
	{
		errmsg = e.what();
	}
	if (!errmsg.empty())
	{
		std::cerr << "Error in configuration: " << errmsg << std::endl;
	}
	return 0;
}


