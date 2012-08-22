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
///\file testWolfilter.cpp
///\brief Test program for wolfilter like stdin/stdout mapping
#include "database/processor.hpp"
#include "logger-v1.hpp"
#include "gtest/gtest.h"
#include "testDescription.hpp"
#include "filter/token_filter.hpp"
#include "filter/textwolf_filter.hpp"
#include "filter/typingfilter.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread/thread.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

static int g_gtest_ARGC = 0;
static char* g_gtest_ARGV[2] = {0, 0};

using namespace _Wolframe;
using namespace _Wolframe::db;

struct DatabaseCommandLog :public DatabaseInterface
{
	DatabaseCommandLog(){}

	virtual bool begin()
	{
		m_out << "begin();" << std::endl;
		return true;
	}

	virtual bool commit()
	{
		m_out << "commit();" << std::endl;
		return true;
	}

	virtual bool rollback()
	{
		m_out << "rollback();" << std::endl;
		return true;
	}

	virtual bool start( const std::string& stmname)
	{
		m_out << "start( '" << stmname << "' );" << std::endl;
		return true;
	}

	virtual bool bind( std::size_t idx, const char* value)
	{
		if (value)
		{
			m_out << "bind( " << idx << ", '" << value << "' );" << std::endl;
		}
		else
		{
			m_out << "bind( " << idx << ", NULL );" << std::endl;
		}
		return true;
	}

	virtual bool execute()
	{
		m_out << "execute();" << std::endl;
		return true;
	}

	virtual unsigned int nofColumns()
	{
		m_out << "nofColumns();" << std::endl;
		return 0;
	}

	virtual const char* columnName( std::size_t idx)
	{
		m_out << "columnName( " << idx << " );" << std::endl;
		return 0;
	}

	virtual const char* getLastError()
	{
		m_out << "getLastError();" << std::endl;
		return 0;
	}

	virtual const char* get( std::size_t idx)
	{
		m_out << "get( " << idx << " );" << std::endl;
		return 0;
	}

	virtual bool next()
	{
		m_out << "next();" << std::endl;
		return 0;
	}

	std::string str()
	{
		return m_out.str();
	}

	void reset()
	{
		m_out.str("");
	}
private:
	std::ostringstream m_out;
};

static DatabaseCommandLog g_databaseCommandLog;

struct TestTransactionFunction :public TransactionFunction
{
	TestTransactionFunction( const std::string& src)
		:TransactionFunction(src){}

	virtual langbind::TransactionFunction::ResultR execute( const langbind::TransactionFunction::Input* inp) const
	{
		DatabaseInterface* dbi = &g_databaseCommandLog;
		return TransactionFunction::execute( dbi, inp);
	}
};

class DatabaseProcessorVMTest : public ::testing::Test
{
protected:
	DatabaseProcessorVMTest() {}
	virtual ~DatabaseProcessorVMTest() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

static std::string selectedTestName;

void pushTestInput( const langbind::TransactionFunction::InputR& input, std::string tdinput)
{
	langbind::Filter tokenfilter = langbind::createTokenFilter( "token:UTF-8");
	langbind::Filter xmlfilter = langbind::createTextwolfXmlFilter( "xml:textwolf");
	langbind::TypingInputFilter inp( xmlfilter.inputfilter());

	xmlfilter.inputfilter()->setValue( "empty", "false");
	xmlfilter.inputfilter()->putInput( tdinput.c_str(), tdinput.size(), true);

	langbind::TypedFilterBase::ElementType type;
	langbind::TypedFilterBase::Element element;
	int taglevel = 0;
	while (inp.getNext( type, element))
	{		
		if (type == langbind::FilterBase::OpenTag)
		{
			taglevel++;
		}
		else if (type == langbind::FilterBase::CloseTag)
		{
			taglevel--;
		}
		if (taglevel >= 0)
		{
			if (!input->print( type, element)) throw std::runtime_error( "token filter buffer overflow");
		}
		else
		{
			const void* ptr;
			std::size_t ii,size;
			bool end;
			xmlfilter.inputfilter()->getRest( ptr, size, end);
			for (ii=0; ii<size; ++ii)
			{
				if (((char*)ptr)[ii] < 0 || ((char*)ptr)[ii] > 32)
				{
					throw std::runtime_error( "unconsumed input left");
				}
			}
			break;
		}
	}
}


TEST_F( DatabaseProcessorVMTest, tests)
{
	std::vector<std::string> tests;
	std::size_t testno;

	// [1] Selecting tests to execute:
	boost::filesystem::recursive_directory_iterator ditr( boost::filesystem::current_path() / "databaseProcessorVM" / "data"), dend;
	if (selectedTestName.size())
	{
		std::cerr << "executing tests matching '" << selectedTestName << "'" << std::endl;
	}

	for (; ditr != dend; ++ditr)
	{
		if (boost::iequals( boost::filesystem::extension( *ditr), ".tst"))
		{
			std::string testname = boost::filesystem::basename(*ditr);
			if (selectedTestName.size())
			{
				if (std::strstr( ditr->path().string().c_str(), selectedTestName.c_str()))
				{
					std::cerr << "selected test '" << testname << "'" << std::endl;
					tests.push_back( ditr->path().string());
				}
			}
			else
			{
				tests.push_back( ditr->path().string());
			}
		}
		else if (!boost::filesystem::is_directory( *ditr))
		{
			std::cerr << "ignoring file '" << *ditr << "'" << std::endl;
		}
	}
	std::sort( tests.begin(), tests.end());

	// [2] Execute tests:
	std::vector<std::string>::const_iterator itr=tests.begin(),end=tests.end();
	for (testno=1; itr != end; ++itr,++testno)
	{
		std::string testname = boost::filesystem::basename(*itr);
		wtest::TestDescription td( *itr);
		if (td.requires.size())
		{
			// [2.2] Skip tests when disabled
			std::cerr << "skipping test '" << testname << "' ( is " << td.requires << ")" << std::endl;
			continue;
		}

		std::cerr << "processing test '" << testname << "'" << std::endl;

		g_databaseCommandLog.reset();
		TestTransactionFunction program( td.config);
		langbind::TransactionFunction::InputR input = program.getInput();
		pushTestInput( input, td.input);
		program.execute( input.get());

		if (td.expected != g_databaseCommandLog.str())
		{
			static boost::mutex mutex;
			boost::interprocess::scoped_lock<boost::mutex> lock(mutex);

			// [2.6] Dump test contents to files in case of error
			boost::filesystem::path OUTPUT( boost::filesystem::current_path() / "temp" / "OUTPUT");
			std::fstream oo( OUTPUT.string().c_str(), std::ios::out | std::ios::binary);
			oo.write( g_databaseCommandLog.str().c_str(), g_databaseCommandLog.str().size());
			if (oo.bad()) std::cerr << "error writing file '" << OUTPUT.string() << "'" << std::endl;
			oo.close();

			boost::filesystem::path EXPECT( boost::filesystem::current_path() / "temp" / "EXPECT");
			std::fstream ee( EXPECT.string().c_str(), std::ios::out | std::ios::binary);
			ee.write( td.expected.c_str(), td.expected.size());
			if (ee.bad()) std::cerr << "error writing file '" << EXPECT.string() << "'" << std::endl;
			ee.close();

			boost::filesystem::path INPUT( boost::filesystem::current_path() / "temp" / "INPUT");
			std::fstream ss( INPUT.string().c_str(), std::ios::out | std::ios::binary);
			ss.write( td.input.c_str(), td.input.size());
			if (ss.bad()) std::cerr << "error writing file '" << INPUT.string() << "'" << std::endl;
			ss.close();

			std::cerr << "test output does not match for '" << *itr << "'" << std::endl;
			std::cerr << "INPUT  written to file '"  << INPUT.string() << "'" << std::endl;
			std::cerr << "OUTPUT written to file '" << OUTPUT.string() << "'" << std::endl;
			std::cerr << "EXPECT written to file '" << EXPECT.string() << "'" << std::endl;

			boost::this_thread::sleep( boost::posix_time::seconds( 3));
		}
		EXPECT_EQ( td.expected, g_databaseCommandLog.str());
	}
}

int main( int argc, char **argv )
{
	g_gtest_ARGC = 1;
	g_gtest_ARGV[0] = argv[0];
	if (argc > 2)
	{
		std::cerr << "too many arguments passed to " << argv[0] << std::endl;
		return 1;
	}
	else if (argc == 2)
	{
		selectedTestName = argv[1];
	}
	::testing::InitGoogleTest( &g_gtest_ARGC, g_gtest_ARGV );
	_Wolframe::log::LogBackend::instance().setConsoleLevel( _Wolframe::log::LogLevel::LOGLEVEL_INFO );
	return RUN_ALL_TESTS();
}


