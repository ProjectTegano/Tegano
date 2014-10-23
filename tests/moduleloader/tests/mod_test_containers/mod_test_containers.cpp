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
//
// a test module
//
#include "config/configurationObject.hpp"
#include "module/moduleEntryPoint.hpp"
#include "module/objectConstructor.hpp"
#include "logger/logger-v1.hpp"
#include "mod_test_containers.hpp"

#error DEPREACATED

namespace _Wolframe {
namespace module {
namespace test_containers {

TestModuleConfig::TestModuleConfig( const std::string& className_, const std::string& configSection_, const std::string& configKeyword_)
	: config::ConfigurationObject( className_, configSection_, configKeyword_)
{
	LOG_DEBUG << "Module: test module config created";
}

bool TestModuleConfig::parse( const config::ConfigurationNode& /*pt*/, const std::string& /*node*/,
    const module::ModuleDirectory* /*modules*/ )
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
	os << indStr << configSection() << ": no config " << std::endl;
}

void TestModuleConfig::setCanonicalPathes( const std::string& /*refPath*/ )
{
}

TestUnitImpl1::TestUnitImpl1( const TestModuleConfig*)
{
	LOG_DEBUG << "Module: TestUnit1 object created";
}

TestUnitImpl2::TestUnitImpl2( const TestModuleConfig*)
{
	LOG_DEBUG << "Module: TestUnit2 object created";
}

TestUnitImpl1::~TestUnitImpl1( )
{
	LOG_DEBUG << "Module: TestUnit1 object destroyed";
}

TestUnitImpl2::~TestUnitImpl2( )
{
	LOG_DEBUG << "Module: TestUnit2 object destroyed";
}

const std::string TestUnitImpl1::hello( )
{
	LOG_ALERT << "MODULE: hello() called";
	return "hello";
}

const std::string TestUnitImpl2::hullo( )
{
	LOG_ALERT << "MODULE: hullo() called";
	return "hullo";
}

bool TestUnitImpl1::resolveDB( const db::DatabaseProviderInterface& /* db */ )
{
	return true;
}

bool TestUnitImpl2::resolveDB( const db::DatabaseProviderInterface& /* db */ )
{
	return true;
}

static const ObjectConstructor* getModule1( void )
{
	static const ConfiguredObjectConstructorTemplate<module::ObjectDescription::TEST_OBJECT, test_containers::TestUnitImpl1, test_containers::TestModuleConfig> mod( "TestObject1", "Test1", "test1");
	return &mod;
}

static const ObjectConstructor* getModule2( void )
{
	static const ConfiguredObjectConstructorTemplate<module::ObjectDescription::TEST_OBJECT, test_containers::TestUnitImpl2, test_containers::TestModuleConfig> mod( "TestObject2", "Test2", "test2");
	return &mod;
}


static const ObjectConstructor* (*containers[])() = {
	getModule1,
	getModule2,
	NULL
};

ModuleEntryPoint entryPoint( 0, "Test Module with containers", containers );

}}} // namespace _Wolframe::module:test_containers
