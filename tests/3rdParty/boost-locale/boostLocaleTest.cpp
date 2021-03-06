//
// testing the boost-locale module with ICU
//

#include "gtest/gtest.h"
#include "wtest/testReport.hpp"

#include <boost/locale/generator.hpp>
#include <boost/locale.hpp>

#include <vector>
#include <string>
#include <algorithm>

// The fixture for testing class _Wolframe::module
class BoostLocaleFixture : public ::testing::Test
{
	protected:
		BoostLocaleFixture( )
		{
			// Get global backend:
			boost::locale::localization_backend_manager my = boost::locale::localization_backend_manager::global();

			// select std backend as default:
			my.select("icu");

			// create a generator that uses this backend:
			boost::locale::generator gen(my);

			// set this backend globally:
			boost::locale::localization_backend_manager::global(my);
		}
};

TEST_F( BoostLocaleFixture, AvailableBackends )
{
	boost::locale::localization_backend_manager my = boost::locale::localization_backend_manager::global();
	std::vector<std::string> backends = my.get_all_backends( );
	std::vector<std::string>::const_iterator begin = backends.begin( ), end = backends.end( );
	if( std::find( begin, end, "icu" ) != end ) {
		SUCCEED( ) ;
	} else {
		FAIL( ) << "Missing icu boost-locale backend!";
	}
}

TEST_F( BoostLocaleFixture, SimpleTestLowerCase )
{
	static const unsigned char input[] = { 'G','r', 0xC3, 0x9C, 0xC3, 0xBC, 'b', 'E', 'l', 0 };
	static const unsigned char must[] = { 'g','r', 0xC3, 0xBC, 0xC3, 0xBC, 'b', 'e', 'l', 0 };

	std::locale loc = boost::locale::generator()("");
	std::string output = boost::locale::to_lower( std::string((const char*)input), loc);
	EXPECT_EQ( output, std::string((const char*)must));
}

TEST_F( BoostLocaleFixture, SimpleTestUpperCase )
{
	static const unsigned char input[] = { 'G','r', 0xC3, 0x9C, 0xC3, 0xBC, 'b', 'E', 'l', 0 };
	static const unsigned char must[] = { 'G','R', 0xC3, 0x9C, 0xC3, 0x9C, 'B', 'E', 'L', 0 };
	std::locale loc = boost::locale::generator()("");
	std::string output = boost::locale::to_upper( std::string((const char*)input), loc);
	EXPECT_EQ( output, std::string((const char*)must));
}

int main( int argc, char **argv )
{
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
