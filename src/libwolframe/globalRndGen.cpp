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
// Global random number generator implementation
//

#include <stdexcept>
#include <cstring>

#ifndef _WIN32
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#else
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#include <wincrypt.h>
#endif

#include "types/globalRngGen.hpp"

namespace _Wolframe	{

#ifdef _WIN32
	#define	DEFAULT_RANDOM_DEVICE	(MS_DEF_PROV)
#else
	#define	DEFAULT_RANDOM_DEVICE	"/dev/urandom";
#endif

GlobalRandomGenerator::GlobalRandomGenerator( const std::string& rndDev )
{
	std::cout << "\nGlobal random generator constructed with device '" << rndDev << "'\n";
	if ( rndDev.empty() )	{
		m_device = DEFAULT_RANDOM_DEVICE;
	}
	else
		m_device = rndDev;
}

GlobalRandomGenerator::GlobalRandomGenerator()
{
	std::cout << "\nGlobal random generator constructed with default device";
	m_device = DEFAULT_RANDOM_DEVICE;
}

unsigned GlobalRandomGenerator::random() const
{
	return 0;
}

void GlobalRandomGenerator::generate( unsigned char* buffer, size_t bytes ) const
{
#ifndef _WIN32
	int hndl = open( m_device.c_str(), O_RDONLY );
	if ( hndl < 0 )	{
		std::string errMsg = "Error opening '" + m_device + "': ";
		throw std::runtime_error( errMsg );
	}

	int rndBytes = read( hndl, buffer, bytes );
	if ( rndBytes < 0 )	{
		std::string errMsg = "Error reading '" + m_device + "'";
		throw std::runtime_error( errMsg );
	}
	else if ( rndBytes < (int)bytes )	{
		std::string errMsg = "Not enough entropy in '" + m_device + "'";
		throw std::logic_error( errMsg );
	}

	close( hndl );
#else
	HCRYPTPROV provider = 0;

	if( !CryptAcquireContext( &provider, 0, m_device.c_str( ), PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT ) ) {
		throw std::runtime_error( "Error opening cyrpto context" );
	}

	if( !CryptGenRandom( provider, bytes, (BYTE *)buffer ) ) {
		CryptReleaseContext( provider, 0 );
		throw std::runtime_error( "Error generating random data" );
	}

	if( !CryptReleaseContext( provider, 0 ) ) {
		throw std::runtime_error( "Error closing cyrpto context" );
	}
#endif
}

} // namespace _Wolframe

