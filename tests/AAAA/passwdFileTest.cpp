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
// paswword file manipulation tests
//

#include "logger-v1.hpp"
#include "gtest/gtest.h"
#include "passwdFile.hpp"

#ifdef _WIN32
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#endif

using namespace _Wolframe::AAAA;
using namespace std;

// make the logger in the modules work although it does not make sense
_Wolframe::log::LogBackend*	logBackendPtr;

// The fixture for testing class _Wolframe::module
class PasswdFileFixture : public ::testing::Test
{
protected:
	PasswdFileFixture( )	{}
};


TEST_F( PasswdFileFixture, parsingLine )
{
	PasswordFile	pwdFile( "passwd", false );
	PwdFileUser	user;
	bool result;

	result = pwdFile.getUser( "Admin", user );
	EXPECT_FALSE( result );

	result = pwdFile.getUser( "admin", user );
	ASSERT_TRUE( result );
	EXPECT_STREQ( "admin", user.user.c_str() );
	EXPECT_STREQ( "1841bac2def7cf53a978f0414aa8d5c3e7c4618899709c84fedcdcd6", user.hash.c_str() );
	EXPECT_STREQ( "Wolframe Administrator", user.info.c_str() );
}

//****************************************************************************

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
