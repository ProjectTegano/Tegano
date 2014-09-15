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
/// \file applicationConfiguration.hpp
/// \brief Application configuration

#ifndef _Wolframe_APPLICATION_CONFIGURATION_HPP_INCLUDED
#define _Wolframe_APPLICATION_CONFIGURATION_HPP_INCLUDED

#include "config/configurationObject.hpp"
#include "moduleDirectoryImpl.hpp"
#include <cstddef>
#include <string>
#include <map>
#include <vector>

namespace _Wolframe {
namespace config {

// forward declarations for configuration elements
class ServerConfiguration;
class ServiceConfiguration;
class LoggerConfiguration;
class BannerConfiguration;
class HandlerConfiguration;
class CmdLineConfiguration;
class ProcProviderConfiguration;
class AaaaProviderConfiguration;
class DatabaseProviderConfiguration;

/// \brief Application configuration structure
class ApplicationConfiguration
{
public:
	std::string configFile;					///< configuration file
	bool foreground;					///< true, if running in foregroud (from command line)
	ServiceConfiguration *serviceCfg;			///< daemon / service configuration
	ServerConfiguration *serverCfg;				///< network server configuration
	LoggerConfiguration *loggerCfg;				///< logger configuration
	config::DatabaseProviderConfiguration *databaseCfg;	///< database provider configuration
	config::AaaaProviderConfiguration *aaaaCfg;		///< AAAA provider configuration
	ProcProviderConfiguration *procCfg;			///< processor configuration
	BannerConfiguration *bannerCfg;				///< banner configuration

public:
	enum ConfigFileType	{
		CONFIG_INFO,
		CONFIG_XML,
		CONFIG_UNDEFINED
	};
	static ConfigFileType configFileType( const std::string& n);

	ApplicationConfiguration();
	~ApplicationConfiguration();

	static ConfigFileType fileType( const std::string& filename, ConfigFileType type );

	bool parseModules( const std::string& filename, ConfigFileType type );
	const std::vector< std::string >& moduleList() const
						{ return m_modFiles; }
	const std::string& moduleFolder() const	{ return m_modFolder; }

	void addModules( const module::ModuleDirectory* modules )
						{ m_modDir = modules; }
	bool parse( const std::string& filename, ConfigFileType type );
	///\brief Finalize configuration for daemon
	void finalize( const CmdLineConfiguration& cmdLine );
	///\brief Finalize configuration for simple program (tests, wolfilter) running in foreground
	void finalize();

	bool check() const;
	bool test() const;
	void print( std::ostream& os ) const;

	static std::string chooseFile( const std::string& globalFile,
					const std::string& userFile,
					const std::string& localFile );
private:
	ConfigFileType				m_type;
	std::vector<ConfigurationObject*>	m_conf;
	std::map<std::string, std::size_t>	m_section;
	std::string				m_modFolder;
	std::vector<std::string>		m_modFiles;

	const module::ModuleDirectory*		m_modDir;
public:
	bool addConfig( const std::string& nodeName, ConfigurationObject* conf );
};

}} // namespace
#endif
