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
/// \file config/loggerConfiguration.hpp
/// \brief Logger configuration structure

#ifndef _Wolframe_CONFIG_LOGGER_HPP_INCLUDED
#define _Wolframe_CONFIG_LOGGER_HPP_INCLUDED

#include "config/configurationBase.hpp"
#include "config/configurationTree.hpp"
#include "logger/logLevel.hpp"
#include "logger/logSyslogFacility.hpp"

#include <string>
#include <list>

namespace _Wolframe {
namespace config {

/// \class LoggerConfiguration
/// \brief Logger configuration
class LoggerConfiguration : public _Wolframe::config::ConfigurationBase
{
public:
	bool			logToStderr;
	log::LogLevel::Level	stderrLogLevel;

	bool			logToFile;
	std::string		logFile;
	log::LogLevel::Level	logFileLogLevel;
	std::string		logFileIdent;
	bool			logToSyslog;
	log::SyslogFacility::Facility syslogFacility;
	log::LogLevel::Level	syslogLogLevel;
	std::string		syslogIdent;
#if defined( _WIN32 )
	bool			logToEventlog;
	std::string		eventlogLogName;
	std::string		eventlogSource;
	log::LogLevel::Level eventlogLogLevel;
#endif // defined( _WIN32 )

	/// constructor
	LoggerConfiguration();

	/// methods
	bool parse( const config::ConfigurationNode& pt, const std::string& node,
		    const module::ModuleDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;

	void setCanonicalPathes( const std::string& referencePath );

//	Not implemented yet, inherited from base for the time being
//	bool test() const;

	void foreground( log::LogLevel::Level debugLevel, bool useConfig );
};

}}//namespace
#endif

