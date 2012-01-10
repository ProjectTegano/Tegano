/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
// application properties - implementation
//

#include "appProperties.hpp"

namespace _Wolframe	{
	enum {APP_MAJOR_VERSION = 0, APP_MINOR_VERSION = 0, APP_REVISION = 5};

	const char*	applicationName()			{ return "iproc"; }
	const Version	applicationVersion()			{ return Version( APP_MAJOR_VERSION, APP_MINOR_VERSION ,APP_REVISION); }

	const char*	config::defaultMainConfig()		{ return "/etc/iproc.conf"; }
	const char*	config::defaultUserConfig()		{ return "~/iproc.conf"; }
	const char*	config::defaultLocalConfig()		{ return "./iproc.conf"; }

	unsigned short	net::defaultTCPport()			{ return 7660; }
	unsigned short	net::defaultSSLport()			{ return 7960; }

	const char*	config::defaultServiceName()		{ return "iproc"; }
#if defined( _WIN32 )
	const char*	config::defaultServiceDisplayName()	{ return "Wolframe iproc Daemon"; }
	const char*	config::defaultServiceDescription()	{ return "a daemon for iprocing"; }
#endif // defined( _WIN32 )

} // namespace _Wolframe

