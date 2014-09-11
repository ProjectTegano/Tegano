//
// testing the Module Loader
//

#include "logger/logger-v1.hpp"
#include "libconfig/moduleDirectoryImpl.hpp"
#include "gtest/gtest.h"
#include "wtest/testReport.hpp"
#include "module/configuredObjectConstructor.hpp"
#include "module/simpleObjectConstructor.hpp"
#include "config/configurationObject.hpp"

#include "tests/mod_test/common.hpp"
#include "tests/mod_test_containers/common.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace _Wolframe::module;
using namespace _Wolframe::log;
using namespace _Wolframe;
using namespace std;

static std::string g_execdir;

// The fixture for testing class _Wolframe::module
class ModuleFixture : public ::testing::Test
{
	protected:
		LogBackend& logBack;

	protected:
		ModuleFixture( ) :
			logBack( LogBackend::instance( ) )
		{
			logBack.setConsoleLevel( LogLevel::LOGLEVEL_DATA );
		}
};

TEST_F( ModuleFixture, LoadingModuleFromDir )
{
	ModuleDirectoryImpl modDir( g_execdir);
	vector<string> modFiles;

#ifndef _WIN32
	modFiles.push_back( "./tests/mod_test/mod_test.so" );
#else
	modFiles.push_back( ".\\tests\\mod_test\\mod_test.dll" );
#endif
	bool res = modDir.loadModules( modFiles );
	ASSERT_TRUE( res );

	const ConfiguredObjectConstructor* builder = modDir.getConfiguredObjectConstructor( "TestObject" );
	ASSERT_TRUE( builder != NULL );

	config::ConfigurationObject* configuration = builder->configuration();
	ASSERT_TRUE( configuration != NULL );

	module::BaseObject* baseobj = builder->object( *configuration );
	test::TestUnit* unit = dynamic_cast<test::TestUnit*>(baseobj);

	string s = unit?unit->hello( ):"";
	ASSERT_EQ( s, "hello" );

	delete configuration;
	delete unit;
}

TEST_F( ModuleFixture, LoadingModuleWithMultipleContainers )
{
	ModuleDirectoryImpl modDir( g_execdir);
	vector<string> modFiles;

#ifndef _WIN32
	modFiles.push_back( "./tests/mod_test_containers/mod_test_containers.so" );
#else
	modFiles.push_back( ".\\tests\\mod_test_containers\\mod_test_containers.dll" );
#endif
	bool res = modDir.loadModules( modFiles );
	ASSERT_TRUE( res );

	const ConfiguredObjectConstructor* builder1 = modDir.getConfiguredObjectConstructor( "TestObject1" );
	ASSERT_TRUE( builder1 != NULL );

	const ConfiguredObjectConstructor* builder2 = modDir.getConfiguredObjectConstructor( "TestObject2" );
	ASSERT_TRUE( builder2 != NULL );

	config::ConfigurationObject* configuration1 = builder1->configuration();
	ASSERT_TRUE( configuration1 != NULL );

	config::ConfigurationObject* configuration2 = builder2->configuration();
	ASSERT_TRUE( configuration2 != NULL );

	module::BaseObject* baseobj1 = builder1->object( *configuration1);
	test_containers::TestUnit1* unit1 = dynamic_cast<test_containers::TestUnit1*>(baseobj1);

	module::BaseObject* baseobj2 = builder2->object( *configuration2);
	test_containers::TestUnit2* unit2 = dynamic_cast<test_containers::TestUnit2*>(baseobj2);
	
	string s1 = unit1?unit1->hello( ):"";
	ASSERT_EQ( s1, "hello" );

	string s2 = unit2?unit2->hullo( ):"";
	ASSERT_EQ( s2, "hullo" );

	delete unit1;
	delete unit2;
}

TEST_F( ModuleFixture, ModuleLogging )
{
	std::stringstream buffer;
	ModuleDirectoryImpl modDir( g_execdir);
	vector<string> modFiles;

#ifndef _WIN32
	modFiles.push_back( "./tests/mod_test/mod_test.so" );
#else
	modFiles.push_back( ".\\tests\\mod_test\\mod_test.dll" );
#endif
	bool res = modDir.loadModules( modFiles );
	ASSERT_TRUE( res );

	const ConfiguredObjectConstructor* builder = modDir.getConfiguredObjectConstructor( "TestObject" );
	ASSERT_TRUE( builder != NULL );

	config::ConfigurationObject* configuration = builder->configuration();
	ASSERT_TRUE( configuration != NULL );

	// Redirect stderr
	std::streambuf *sbuf = std::cerr.rdbuf();	// Save stderr's buffer here
	std::cerr.rdbuf( buffer.rdbuf() );

	module::BaseObject* baseobj = builder->object( *configuration);
	test::TestUnit* unit = dynamic_cast<test::TestUnit*>(baseobj);

	string s = unit?unit->hello( ):"";
	ASSERT_EQ( s, "hello" );
	ASSERT_EQ( buffer.rdbuf()->str(), "DEBUG: Module: testUnit object created\n"
					"ALERT: MODULE: hello() called\n" );

	std::cout << buffer.rdbuf();
	// Restore stderr buffer
	std::cerr.rdbuf( sbuf );

	delete configuration;
	delete unit;
}

TEST_F( ModuleFixture, LoadingMissingModuleFile )
{
	ModuleDirectoryImpl modDir( g_execdir);
	vector<string> modFiles;

#ifndef _WIN32
	modFiles.push_back( "./tests/not_there/not_there.so" );
#else
	modFiles.push_back( ".\\tests\\not_there\\not_there.dll" );
#endif
	bool res = modDir.loadModules( modFiles );
	ASSERT_FALSE( res );
}

TEST_F( ModuleFixture, LoadingNotAWolframeModule )
{
	ModuleDirectoryImpl modDir( g_execdir);
	vector<string> modFiles;

#ifndef _WIN32
	modFiles.push_back( "./tests/not_a_mod/not_a_mod.so" );
#else
	modFiles.push_back( ".\\tests\\not_a_mod\\not_a_mod.dll" );
#endif
	bool res = modDir.loadModules( modFiles );
	ASSERT_FALSE( res );
}

TEST_F( ModuleFixture, LoadingModuleWithoutExtension )
{
	ModuleDirectoryImpl modDir( g_execdir);
	vector<string> modFiles;

#ifndef _WIN32
	modFiles.push_back( "./tests/mod_test/mod_test" );
#else
	modFiles.push_back( ".\\tests\\mod_test\\mod_test" );
#endif
	bool res = modDir.loadModules( modFiles );
	ASSERT_TRUE( res );
}

TEST_F( ModuleFixture, LoadingModuleLackingASymbol )
{
	ModuleDirectoryImpl modDir( g_execdir);
	vector<string> modFiles;

#ifndef _WIN32
	modFiles.push_back( "./tests/missing_symbol/missing_symbol.so" );
#else
	modFiles.push_back( ".\\tests\\missing_symbol\\missing_symbol.dll" );
#endif
	bool res = modDir.loadModules( modFiles );
	ASSERT_FALSE( res );
}

TEST_F( ModuleFixture, LoadingModuleResolvableSymbol )
{
	ModuleDirectoryImpl modDir( g_execdir);
	vector<string> modFiles;

#ifdef _WIN32
	SetDllDirectory( "tests\\libbla" );
#endif

#ifndef _WIN32
	modFiles.push_back( "./tests/resolvable_symbol/resolvable_symbol.so" );
#else
	modFiles.push_back( ".\\tests\\resolvable_symbol\\resolvable_symbol.dll" );
#endif
	bool res = modDir.loadModules( modFiles );
	ASSERT_TRUE( res );
}

int main( int argc, char **argv )
{
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
