//
// appConfig.cpp
//

#include "appConfig.hpp"
#include "configStandard.hpp"
#include "commandLine.hpp"

#include "miscUtils.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

#include <boost/algorithm/string.hpp>

#include <string>
#include <ostream>


namespace _SMERP {
	namespace Configuration {

ApplicationConfiguration::ApplicationConfiguration()
{
	// server
	srvConfig = new Configuration::ServerConfiguration( "server", "Server" );

	// logging
	logConfig = new Configuration::LoggerConfiguration( "logging", "Logging" );

//	handlerConfig = new Configuration::DatabaseConfiguration( "database", "Database Server" );

	confs["server"] = srvConfig;
	confs["logging"] = logConfig;
//	confs["database"] = handlerConfig;
}


const char* ApplicationConfiguration::chooseFile( const char *globalFile, const char *userFile, const char *localFile )
{
	if ( globalFile != NULL )
		if ( boost::filesystem::exists( globalFile ))
			return globalFile;
	if ( userFile != NULL )
		if ( boost::filesystem::exists( userFile ))
			return userFile;
	if ( localFile != NULL )
		if ( boost::filesystem::exists( localFile ))
			return localFile;
	return NULL;
}


bool ApplicationConfiguration::parse ( const char *filename, std::ostream& os )
{
	configFile = resolvePath( boost::filesystem::absolute( filename ).string() );
	if ( !boost::filesystem::exists( configFile ))	{
		os << "Configuration file " << configFile << " does not exist.";
		return false;
	}

	// Create an empty property tree object
	boost::property_tree::ptree	pt;
	try	{
		read_info( filename, pt );

		for ( boost::property_tree::ptree::const_iterator it = pt.begin();
								it != pt.end(); it++ )	{
			std::map< std::string, ConfigurationBase* >::iterator confIt;
			if (( confIt = confs.find( it->first ) ) != confs.end() )	{
				if ( ! confIt->second->parse( pt.get_child( it->first ), os ))
					return false;
			}
//			else	{
//				os << "Unknown configuration option " << it->first;
//				return false;
//			}
		}

	}
	catch( std::exception& e)	{
		os << e.what();
		return false;
	}
	return true;
}


void ApplicationConfiguration::finalize( const CmdLineConfig& cmdLine )
{
#if defined(_WIN32)
	// on Windows the user should either use -f or start in the console
	// (assuming an implicit -f) or the service option should contain
	// --service in the startup parameters of the service impling non-foreground
	if( cmdLine.command == _SMERP::CmdLineConfig::RUN_SERVICE )
		foreground = false;
	else
		foreground = true;
#else
	foreground = cmdLine.foreground;
#endif
	if ( foreground )
		logConfig->foreground( cmdLine.debugLevel, cmdLine.useLogConfig );
#if !defined(_WIN32)
	srvConfig->override( cmdLine.user, cmdLine.group );
#endif
}


void ApplicationConfiguration::print( std::ostream& os ) const
{

	os << "Configuration file: " << configFile << std::endl;
	// Unix daemon
#if !defined(_WIN32)
	os << "Run in foreground: " << (foreground ? "yes" : "no") << std::endl;
#endif
	for ( std::map< std::string, ConfigurationBase* >::const_iterator it = confs.begin();
									it != confs.end(); it++ )
		it->second->print( os );
}

/// Check if the application configuration makes sense
bool ApplicationConfiguration::check( std::ostream& os ) const
{
	// check logging
	if ( ! logConfig->check( os ))
		return false;
	return true;
}

	} // namespace Configuration
} // namespace _SMERP
