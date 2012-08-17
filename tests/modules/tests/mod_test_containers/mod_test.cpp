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
//
// a test module
//

#include "moduleInterface.hpp"
#include "logger-v1.hpp"
#include "mod_test.hpp"

_Wolframe::log::LogBackend*	logBackendPtr;

namespace _Wolframe {
namespace module {
namespace test_containers {

TestModuleConfig::TestModuleConfig( const char* cfgName, const char* logParent, const char* logName )
	: config::ObjectConfiguration( cfgName, logParent, logName )
{
	MOD_LOG_DEBUG << "Test module config created";
}

bool TestModuleConfig::parse( const config::ConfigurationTree& /*pt*/, const std::string& /*node*/,
    const module::ModulesDirectory* /*modules*/ )
{
	return true;
}

bool TestModuleConfig::check( ) const
{
	return true;
}

void TestModuleConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );
	os << indStr << sectionName() << ": no config " << std::endl;
}

void TestModuleConfig::setCanonicalPathes( const std::string& /*refPath*/ )
{
}

TestModuleContainer1::TestModuleContainer1( const TestModuleConfig& /* conf */ )
{
	m_test = new TestUnitImpl1( /* conf */ );
	MOD_LOG_DEBUG << "Test module 1 container created";
}

TestModuleContainer2::TestModuleContainer2( const TestModuleConfig& /* conf */ )
{
	m_test = new TestUnitImpl2( /* conf */ );
	MOD_LOG_DEBUG << "Test module 2 container created";
}

TestUnitImpl1::TestUnitImpl1( )
{
	MOD_LOG_DEBUG << "TestUnit1 object created";
}

TestUnitImpl2::TestUnitImpl2( )
{
	MOD_LOG_DEBUG << "TestUnit2 object created";
}

TestUnitImpl1::~TestUnitImpl1( )
{
	MOD_LOG_DEBUG << "TestUnit1 object destroyed";
}

TestUnitImpl2::~TestUnitImpl2( )
{
	MOD_LOG_DEBUG << "TestUnit2 object destroyed";
}

const std::string TestUnitImpl1::hello( )
{
	return "hello";
}

const std::string TestUnitImpl2::hullo( )
{
	return "hullo";
}

bool TestUnitImpl1::resolveDB( const db::DatabaseProvider& /* db */ )
{
	return true;
}

bool TestUnitImpl2::resolveDB( const db::DatabaseProvider& /* db */ )
{
	return true;
}

static ContainerBuilder* createModule1( void )
{
	static module::ContainerDescription< test_containers::TestModuleContainer1,
		test_containers::TestModuleConfig > mod( "Test Module 1", "Test 1", "test1", "TestObject1" );
	return &mod;
}

static ContainerBuilder* createModule2( void )
{
	static module::ContainerDescription< test_containers::TestModuleContainer2,
		test_containers::TestModuleConfig > mod( "Test Module 2", "Test 2", "test 2", "TestObject2" );
	return &mod;
}

static void setModuleLogger( void* logger )
{
	logBackendPtr = reinterpret_cast< _Wolframe::log::LogBackend* >( logger );
}


static const unsigned short nrContainers = 2;
static ContainerBuilder* (*containers[ nrContainers ])() = {
	createModule1,
	createModule2
};

ModuleEntryPoint entryPoint( 0, "Test Module with containers", setModuleLogger,
			     nrContainers, containers,
			     0, NULL );

}}} // namespace _Wolframe::module:test_containers
