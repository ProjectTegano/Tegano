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
// text file authenticator tests
//

#include "logger/logger-v1.hpp"
#include "gtest/gtest.h"

#include "libconfig/moduleDirectoryImpl.hpp"
#include "libconfig/aaaaProviderConfiguration.hpp"
#include "libprovider/randomGeneratorImpl.hpp"
#include "libprovider/aaaaProviderImpl.hpp"
#include "appdevel/staticFrameMacros.hpp"
#include "appdevel/authenticationModuleMacros.hpp"
#include "TextFileAuth.hpp"
#include "types/base64.hpp"
#include "crypto/HMAC.hpp"
#include "aaaa/passwordHash.hpp"
#include "aaaa/CRAM.hpp"
#include "modules/authentication/textfile/TextFileAuth.hpp"
#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

static std::string g_execdir;
_Wolframe::system::RandomGeneratorImpl g_randomGenerator;

using namespace _Wolframe::aaaa;
using namespace _Wolframe::log;
using namespace _Wolframe::module;

using namespace _Wolframe;
using namespace std;

WF_STATIC_FRAME_BEGIN( TextFileAuthentication);
WF_AUTHENTICATOR( "TextFileAuthenticator", aaaa::TextFileAuthUnit, aaaa::TextFileAuthConfig)
WF_STATIC_FRAME_END

// The fixture for testing class _Wolframe::module
class AuthenticatorFixture : public ::testing::Test
{
protected:
	LogBackend&		logBack;

	AuthenticatorFixture( ) :
		logBack( LogBackend::instance( ) )
	{
		// Set the log level
		logBack.setConsoleLevel( LogLevel::LOGLEVEL_INFO );

		// Build the modules directory
		ModuleDirectoryImpl modDir( g_execdir);
		TextFileAuthentication mod;
		const ConfiguredObjectConstructor* auth = dynamic_cast<const ConfiguredObjectConstructor*>(&mod[0]);
		modDir.addObjectConstructor( auth);
		config::AaaaProviderConfiguration cfg;
		aaaa::AaaaProviderImpl aaaaProvider( &g_randomGenerator, cfg.authConfig(), cfg.authzConfig(), cfg.authzDefault(), cfg.auditConfig(), &modDir);
	}
};

//****************************************************************************

int main( int argc, char **argv )
{
	g_execdir = boost::filesystem::system_complete( argv[0]).parent_path().string();
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
