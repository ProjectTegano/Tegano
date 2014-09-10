//
// handlerConfig.hpp
//

#ifndef _HANDLERCONFIG_HPP_INCLUDED
#define _HANDLERCONFIG_HPP_INCLUDED

#include "standardConfigs.hpp"

namespace _Wolframe {

	/// echo configuration
	struct EchoConfiguration : public config::ConfigurationObject
	{
	public:
		unsigned short		timeout;

		/// constructor
		EchoConfiguration() : ConfigurationObject( "EchoServer", "", "" )
								{ timeout = 0; }
		/// methods
		bool parse( const config::ConfigurationNode& pt, const std::string& node,
			    const module::ModuleDirectory* modules );
		bool check() const;
		void print( std::ostream& os, size_t indent ) const;

//		Not implemented yet, inherited from base for the time being
//		bool test() const;
	};


	/// Wolframe handler configuration structure
	struct HandlerConfiguration
	{
	public:
		EchoConfiguration	*echoConfig;

		/// constructor
		HandlerConfiguration()
		{
			echoConfig = new EchoConfiguration();
		}

		~HandlerConfiguration()
		{
			if ( echoConfig ) delete echoConfig;
		}
	};


} // namespace _Wolframe

#endif // _HANDLERCONFIG_HPP_INCLUDED
