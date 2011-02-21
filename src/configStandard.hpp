//
// standard configuration structures
//

#ifndef _CONFIG_STANDARD_HPP_INCLUDED
#define _CONFIG_STANDARD_HPP_INCLUDED

#include "appDefaults.hpp"
#include "configurationBase.hpp"
#include "serverEndpoint.hpp"
#include "logLevel.hpp"

#if !defined( _WIN32 )
#include "logSyslogFacility.hpp"
#endif	// !defined( _WIN32 )

#include <string>
#include <list>


namespace _SMERP {
	namespace Configuration	{

		/// server configuration
		struct ServerConfiguration : public _SMERP::Configuration::ConfigurationBase	{
		public:
#if !defined( _WIN32 )
			// daemon configuration
			std::string		user;
			std::string		group;
			std::string		pidFile;
#else
			// service configuration
			std::string		serviceName;
			std::string		serviceDisplayName;
			std::string		serviceDescription;
#endif // !defined( _WIN32 )
			// server configuration
			unsigned short		threads;

			unsigned short		maxConnections;

			// network configuration
			std::list<Network::ServerTCPendpoint> address;
			std::list<Network::ServerSSLendpoint> SSLaddress;

			/// constructor
			ServerConfiguration( const std::string& node, const std::string& header );

			/// methods
			bool parse( boost::property_tree::ptree& pt, std::ostream& os );
			bool check( std::ostream& os ) const;
			void print( std::ostream& os ) const;

//			Not implemented yet, inherited from base for the time being
//			bool test( std::ostream& os ) const;

#if !defined( _WIN32 )
			void override( const std::string& user, const std::string& group );
#endif // !defined( _WIN32 )
		};


		/// logger configuration
		struct LoggerConfiguration : public _SMERP::Configuration::ConfigurationBase
		{
		public:
			bool			logToStderr;
			LogLevel::Level		stderrLogLevel;

			bool			logToFile;
			std::string		logFile;
			LogLevel::Level		logFileLogLevel;
			std::string		logFileIdent;
#if !defined( _WIN32 )
			bool			logToSyslog;
			SyslogFacility::Facility syslogFacility;
			LogLevel::Level		syslogLogLevel;
			std::string		syslogIdent;
#else
			bool			logToEventlog;
			std::string		eventlogLogName;
			std::string		eventlogSource;
			LogLevel::Level		eventlogLogLevel;
#endif // !defined( _WIN32 )

			/// constructor
			LoggerConfiguration( const std::string& node, const std::string& header );

			/// methods
			bool parse( boost::property_tree::ptree& pt, std::ostream& os );
			bool check( std::ostream& os ) const;
			void print( std::ostream& os ) const;

//			Not implemented yet, inherited from base for the time being
//			bool test( std::ostream& os ) const;

			void foreground( LogLevel::Level debugLevel, bool useConfig );
		};


		/// database configuration
		struct DatabaseConfiguration : public _SMERP::Configuration::ConfigurationBase
		{
		public:
			std::string		host;
			unsigned short		port;
			std::string		name;
			std::string		user;
			std::string		password;

			/// constructor
			DatabaseConfiguration( const std::string& node, const std::string& header )
				: ConfigurationBase( node, header )	{ port = 0; }
			/// methods
			bool parse( boost::property_tree::ptree& pt, std::ostream& os );
			bool check( std::ostream& os ) const;
			void print( std::ostream& os ) const;

//			Not implemented yet, inherited from base for the time being
//			bool test( std::ostream& os ) const;
		};

	} // namespace Configuration
} // namespace _SMERP


#endif // _CONFIG_STANDARD_HPP_INCLUDED
