#include "appdevel/authenticationModuleMacros.hpp"
#include "appdevel/moduleFrameMacros.hpp"
#include "serialize/descriptiveConfiguration.hpp"
#include "aaaa/authenticationSlice.hpp"
#include "aaaa/authenticator.hpp"
#include <string>

class MyAuthenticationConfig
	:public _Wolframe::serialize::DescriptiveConfiguration
{
public:
	// ... define your configuration data members here

	static const _Wolframe::serialize::StructDescriptionBase* getStructDescription()
	{
		// ... return your introspection description reference of the configuration here
		return 0;
	}
	MyAuthenticationConfig( const char* title, const char* logprefix, const char* /*subsection*/)
		:_Wolframe::serialize::DescriptiveConfiguration( title, "authentication", logprefix, getStructDescription())
	{
		setBasePtr( (void*)this); // ... mandatory to set pointer to start of configuration
	}
};


class MyAuthenticatorSlice
	:public _Wolframe::aaaa::AuthenticatorSlice
{
public:
	virtual void dispose()
	{
		// ... destroy the object accordingly to the method is was created by the unit method _Wolframe::aaaa::AuthenticatorUnit::slice( const std::string&, const _Wolframe::net::RemoteEndpoint&)
	}

	virtual const char* className() const
	{
		return "MyAuthenticatorSlice";
	}

	virtual const std::string& identifier() const
	{
		static const std::string my_authenticatorID("myauth");
		return my_authenticatorID;  // ... return the configuration identifier of your authenticator
	}

	virtual void messageIn( const std::string& /*msg*/)
	{
		// ... process the message requested by 'status()const' here
	}

	virtual std::string messageOut()
	{
		// ... return the message to be sent announced by 'status()const' here
		return std::string();
	}

	virtual _Wolframe::aaaa::AuthenticatorSlice::Status status() const
	{
		// ... return the current status of the authenticator slice
		return (_Wolframe::aaaa::AuthenticatorSlice::Status)0;
	}

	virtual bool inputReusable() const
	{
		// ... return true, if the last message processed can be forwarded to another slice of the same mech
		return false;
	}

	virtual _Wolframe::aaaa::User* user()
	{
		// ... 	return the authenticated user or NULL if not authenticated here
		return 0;
	}
};

class MyAuthenticationUnit
	:public _Wolframe::aaaa::AuthenticationUnit
{
public:
	MyAuthenticationUnit( const MyAuthenticationConfig*)
		:_Wolframe::aaaa::AuthenticationUnit("MyAuthenticationId")
	{
		// ... initialize this authentication unit here
	}

	virtual const char* className() const
	{
		return "MyAuthentication";
	}

	const char** mechs() const
	{
		// ... return the mechs implemented by this authenticator unit here
		return 0;
	}

	MyAuthenticatorSlice* slice( const std::string& /*mech*/, const _Wolframe::net::RemoteEndpoint& /*client*/)
	{
		// ... create and return a new instance of an authenticator slice here
		return 0;
	}
};

WF_MODULE_BEGIN( "MyAuthenticator", "my authenticator module")
 WF_AUTHENTICATOR( "my authenticator", MyAuthenticationUnit, MyAuthenticationConfig)
WF_MODULE_END

