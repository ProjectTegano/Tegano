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
/// \file serverHandler.cpp

#include "serverHandler.hpp"
#include "serverConnectionHandler.hpp"
#include "libconfig/bannerConfiguration.hpp"
#include "libconfig/AAAAproviderConfiguration.hpp"
#include "libconfig/procProviderConfiguration.hpp"
#include "libprovider/procProviderImpl.hpp"
#include "libprovider/AAAAproviderImpl.hpp"
#include "prgbind/programLibrary.hpp"
#include "logger/logger-v1.hpp"
#include <stdexcept>

#include <string>
#include <cstring>
#include <stdexcept>

using namespace _Wolframe;

/// ServerHandler PIMPL
net::ConnectionHandler* ServerHandler::newConnection( const net::LocalEndpointR& local )
{
	return new ServerConnectionHandler( this, local );
}

ServerHandler::ServerHandler( const config::ProcProviderConfiguration* pconf,
				const config::AAAAproviderConfiguration* aconf,
				const db::DBproviderConfig* dconf,
				const config::BannerConfiguration* bconf,
				system::RandomGenerator* randomGenerator_,
				const module::ModuleDirectory* modules )
	:m_banner( bconf->toString() )
	,m_db( dconf, modules )
	,m_aaaa( randomGenerator_, aconf->authConfig(), aconf->authzConfig(), aconf->authzDefault(), aconf->auditConfig(), modules )
	,m_proc( pconf->dbLabel(), pconf->procConfig(), pconf->programFiles(), pconf->referencePath(), modules)
	,m_randomGenerator(randomGenerator_)
	{}

ServerHandler::~ServerHandler()	{}

