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
// objectPool template unit tests
//

#include <cstdlib>
#include <cassert>
#include <boost/thread/thread.hpp>

#include "types/objectPool.hpp"
#include "gtest/gtest.h"
#include "wtest/testReport.hpp"

using namespace _Wolframe;

class TestObject	{
public:
	TestObject()		{ m_threads = 0, m_uses = 0; }
	void doSomething()	{
		assert( m_threads == 0 );
		m_threads++, m_uses++;
		assert( m_threads == 1 );
		m_threads--;
		assert( m_threads == 0 );
	}
	void sleepSomething()	{
		assert( m_threads == 0 );
		m_threads++, m_uses++;
		assert( m_threads == 1 );
		boost::this_thread::sleep( boost::posix_time::microseconds( rand() % 150 ));
		assert( m_threads == 1 );
		m_threads--;
		assert( m_threads == 0 );
	}
	bool unused()		{ return m_threads == 0; }
	int threads()		{ return m_threads; }
	unsigned long used()	{ return m_uses; }
private:
	int		m_threads;
	unsigned long	m_uses;
};


// The fixture for testing Wolframe::types::ObjectPool template
class ObjectPoolFixture : public ::testing::Test	{
protected:
	// Set-up work for each test here.
	ObjectPoolFixture()	{
		srand(clock());
		noThreads = 1 + (int)( rand() % 32 );
		poolSize = 1 + (int)( rand() % 16 );
		times = (unsigned long)( rand() % 200000 );
		std::cout << noThreads << " threads, " << poolSize << " elements in the pool, "
			  << times << " iterations, total "<< noThreads * times << std::endl;
	}


	// Clean-up work that doesn't throw exceptions here.
	virtual ~ObjectPoolFixture()	{
	}

	// Code here will be called immediately after the constructor (right
	// before each test).
	virtual void SetUp() {
		for ( unsigned long i = 0; i < poolSize; i++ )	{
			TestObject *tstObj = new TestObject();
			objPool.push( tstObj );
		}
	}

	// Code here will be called immediately after each test (right
	// before the destructor).
	virtual void TearDown()
	{}

	typedef std::vector<types::ObjectPool<TestObject>::ObjectRef> ObjectList;
	ObjectList objects()
	{
		ObjectList rt;
		while (objPool.size())
		{
			rt.push_back( objPool.get());
		}
		return rt;
	}

	// Objects declared here can be used by all tests in the test case.
	int					noThreads;
	std::vector< boost::thread* >		threads;
	types::ObjectPool<TestObject>		objPool;
	unsigned				poolSize;
	unsigned long				times;
	bool					emptyPool;

public:
	static void testThread( types::ObjectPool<TestObject> *pool, unsigned count )
	{
		for ( std::size_t i = 0; i < count; )	{
			try	{
				types::ObjectPool<TestObject>::ObjectRef tstObj = pool->get();
				tstObj->doSomething();
				i++;
			}
			catch ( types::ObjectPoolTimeoutException )
			{
			}
		}
	}

	static void sleepTestThread( types::ObjectPool<TestObject> *pool, unsigned count )
	{
		for ( std::size_t i = 0; i < count; )	{
			try	{
				types::ObjectPool<TestObject>::ObjectRef tstObj = pool->get();
				tstObj->sleepSomething();
				i++;
			}
			catch ( types::ObjectPoolTimeoutException )
			{
			}
		}
	}

};


// Tests the ObjectPool get & release
TEST_F( ObjectPoolFixture, noTimeout )	{
	for ( int i = 0; i < noThreads; i++ )   {
		boost::thread* thread = new boost::thread( &ObjectPoolFixture::testThread, &objPool, times );
		threads.push_back( thread );
	}
	for ( int i = 0; i < noThreads; i++ )   {
		threads[i]->join();
		delete threads[i];
	}
	ASSERT_EQ( objPool.size(), poolSize );
	ObjectList tstObjs = objects();

	unsigned long used = 0;
	for ( unsigned long i = 0; i < poolSize; i++ )	{
		ASSERT_TRUE( tstObjs[i]->unused() );
		used += tstObjs[i]->used();
	}

	for ( unsigned long i = 0; i < poolSize; i++ )
		ASSERT_EQ( tstObjs[i]->threads(), 0 );
	ASSERT_EQ( used, noThreads * times );
	emptyPool = false;
}

TEST_F( ObjectPoolFixture, noTimeoutSleep )	{
	for ( int i = 0; i < noThreads; i++ )   {
		boost::thread* thread = new boost::thread( &ObjectPoolFixture::sleepTestThread, &objPool, times );
		threads.push_back( thread );
	}
	for ( int i = 0; i < noThreads; i++ )   {
		threads[i]->join();
		delete threads[i];
	}
	ASSERT_EQ( objPool.size(), poolSize );
	ObjectList tstObjs = objects();

	unsigned long used = 0;
	for ( unsigned long i = 0; i < poolSize; i++ )	{
		ASSERT_TRUE( tstObjs[i]->unused() );
		used += tstObjs[i]->used();
	}

	for ( unsigned long i = 0; i < poolSize; i++ )
		ASSERT_EQ( tstObjs[i]->threads(), 0 );
	ASSERT_EQ( used, noThreads * times );
	emptyPool = false;
}

TEST_F( ObjectPoolFixture, Timeout )	{
	objPool.setTimeout( 1 );
	for ( int i = 0; i < noThreads; i++ )   {
		boost::thread* thread = new boost::thread( &ObjectPoolFixture::testThread, &objPool, times );
		threads.push_back( thread );
	}
	for ( int i = 0; i < noThreads; i++ )   {
		threads[i]->join();
		delete threads[i];
	}

	ASSERT_EQ( objPool.size(), poolSize );
	ObjectList tstObjs = objects();

	unsigned long used = 0;
	for ( unsigned long i = 0; i < poolSize; i++ )	{
		ASSERT_TRUE( tstObjs[i]->unused() );
		used += tstObjs[i]->used();
	}

	for ( unsigned long i = 0; i < poolSize; i++ )
		ASSERT_EQ( tstObjs[i]->threads(), 0 );
	ASSERT_EQ( used, noThreads * times );
	emptyPool = false;
}

TEST_F( ObjectPoolFixture, TimeoutSleep )	{
	objPool.setTimeout( 1 );
	for ( int i = 0; i < noThreads; i++ )   {
		boost::thread* thread = new boost::thread( &ObjectPoolFixture::sleepTestThread, &objPool, times );
		threads.push_back( thread );
	}
	for ( int i = 0; i < noThreads; i++ )   {
		threads[i]->join();
		delete threads[i];
	}

	ASSERT_EQ( objPool.size(), poolSize );
	ObjectList tstObjs = objects();

	unsigned long used = 0;
	for ( unsigned long i = 0; i < poolSize; i++ )	{
		ASSERT_TRUE( tstObjs[i]->unused() );
		used += tstObjs[i]->used();
	}

	for ( unsigned long i = 0; i < poolSize; i++ )
		ASSERT_EQ( tstObjs[i]->threads(), 0 );
	ASSERT_EQ( used, noThreads * times );
	emptyPool = false;
}

TEST_F( ObjectPoolFixture, TestTimeout )	{
	objPool.setTimeout( 1 );

	std::vector<types::ObjectPool<TestObject>::ObjectRef> ar;

	for ( unsigned long i = 0; i < poolSize; i++ )
	{
		ar.push_back( objPool.get());
	}
	ASSERT_THROW( objPool.get(), types::ObjectPoolTimeoutException );
	emptyPool = true;
}

int main( int argc, char **argv )
{
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
