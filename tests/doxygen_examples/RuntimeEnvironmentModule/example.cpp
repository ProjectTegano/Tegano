#include "appdevel/runtimeEnvironmentModuleMacros.hpp"
#include "appdevel/moduleFrameMacros.hpp"
#include "serialize/descriptiveConfiguration.hpp"

static int initMyRuntimeEnvironment()
{
	// ... put your global initializations here
	// ... return 0 on success, an error code != 0 else
}

class MyRuntimeEnvironmentConfig
	:public _Wolframe::serialize::DescriptiveConfiguration
{
public:
	// ... define your runtime environment configuration data members here

	static const _Wolframe::serialize::StructDescriptionBase* getStructDescription()
	{
		// ... return your introspection description reference of the configuration here
	}
	MyRuntimeEnvironmentConfig( const std::string& className_, const std::string& configSection_, const std::string& configKeyword_)
		:_Wolframe::serialize::DescriptiveConfiguration( className_, configSection_, configKeyword_, getStructDescription())
	{
		setBasePtr( (void*)this); // ... mandatory to set pointer to start of configuration
	}
};

class MyRuntimeEnvironment
	:public _Wolframe::langbind::RuntimeEnvironment
{
public:
	// ... put your runtime environment host structures here

	MyRuntimeEnvironment( const MyRuntimeEnvironmentConfig* cfg)
	{
		// ... create your runtime environment from its configuration here
	}
	virtual _Wolframe::langbind::FormFunctionClosure* createClosure( const std::string& funcname) const
	{
		// ... create and return a closure to execute the function 'funcname' here
	}

	virtual std::vector<std::string> functions() const
	{
		// ... return the list of functions that are exported by the runtime environment here
	}

	virtual const char* name() const
	{
		// ... return the name of the runtime environment here
	}
};

WF_MODULE_BEGIN( "MyRuntimeEnvironment", "runtime environment for my programs")
 WF_RUNTIME_ENVIRONMENT( "my runtime environment", "runtimeenv", "myrunenv", MyRuntimeEnvironment, MyRuntimeEnvironmentConfig, initMyRuntimeEnvironment)
WF_MODULE_END
