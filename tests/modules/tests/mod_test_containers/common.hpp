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
// common part which has to be known to Wolframe and the loadable module
//

#include "moduleInterface.hpp"
#include "database/DBprovider.hpp"

namespace _Wolframe {
namespace module {
namespace test_containers {

class TestModuleConfig :  public config::NamedConfiguration
{
	friend class TestModuleContainer;

public:
	TestModuleConfig( const char* cfgName, const char* logParent, const char* logName );

	virtual ~TestModuleConfig( ) {}

	virtual const char* objectName() const		{ return "TestUnit"; }

	/// methods
	bool parse( const config::ConfigurationTree& pt, const std::string& node,
		    const module::ModulesDirectory* modules );

	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );
private:
	std::string m_a_param;
};

// must be an abstract base class for usage in the code loading
// and using objects of the derived class!
class TestUnit1
{
public:
	// may not be private and must be virtual, can be empty as
	// we define an interface here only
	virtual ~TestUnit1( ) { }

	// must be virtual, otherwise moduleTest tries to link a hello
	// function in which can't exist there!
	// must be abstract too, otherwise we get a dlopen error with
	// 'typeinfo for _Wolframe::module::test::TestUnit' missing
	virtual const std::string hello( ) = 0;
};

class TestUnit2
{
public:
	// may not be private and must be virtual, can be empty as
	// we define an interface here only
	virtual ~TestUnit2( ) { }

	// must be virtual, otherwise moduleTest tries to link a hello
	// function in which can't exist there!
	// must be abstract too, otherwise we get a dlopen error with
	// 'typeinfo for _Wolframe::module::test::TestUnit' missing
	virtual const std::string hullo( ) = 0;
};

class TestModuleContainer1 : public ObjectContainer< TestUnit1 >
{
public:
	TestModuleContainer1( const TestModuleConfig& conf );

	~TestModuleContainer1()			{}

	virtual const char* identifier() const	{ return "TestUnit1"; }
	virtual TestUnit1* object() const	{ return m_test; }
	void dispose()				{ m_test = NULL; delete this; }
private:
	TestUnit1 *m_test;
};

class TestModuleContainer2 : public ObjectContainer< TestUnit2 >
{
public:
	TestModuleContainer2( const TestModuleConfig& conf );

	~TestModuleContainer2()			{}

	virtual const char* identifier() const	{ return "TestUnit2"; }
	virtual TestUnit2* object() const	{ return m_test; }
	void dispose()				{ m_test = NULL; delete this; }
private:
	TestUnit2 *m_test;
};

}}} // namespace _Wolframe::module::test_containers
