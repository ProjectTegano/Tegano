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
/// \file serverHandler.hpp

#ifndef _Wolframe_SERVER_HANDLER_HPP_INCLUDED
#define _Wolframe_SERVER_HANDLER_HPP_INCLUDED

#include "serverConnectionHandler.hpp"
#include "system/connectionHandler.hpp"
#include "processor/execContext.hpp"
#include "cmdbind/protocolHandler.hpp"
#include "database/database.hpp"
#include "processor/execContext.hpp"
#include "mainConnectionHandler.hpp"
#include "config/bannerConfiguration.hpp"
#include "AAAA/AAAAprovider.hpp"
#include "provider/procProviderImpl.hpp"

namespace _Wolframe {

/// \brief The server handler container
class ServerHandler
{
public:
	ServerHandler( const proc::ProcProviderConfig* pconf,
			const AAAA::AAAAconfiguration* aconf,
			const db::DBproviderConfig* dconf,
			const config::BannerConfiguration* bconf,
			const module::ModulesDirectory* modules);
	~ServerHandler();
	
	net::ConnectionHandler* newConnection( const net::LocalEndpointR& local );

	const std::string& banner() const			{ return m_banner; }
	const db::DatabaseProvider* dbProvider() const		{ return &m_db; }
	const AAAA::AAAAprovider* aaaaProvider() const		{ return &m_aaaa; }
	const proc::ProcessorProvider* procProvider() const	{ return &m_proc; }

private:
	const std::string		m_banner;
	db::DatabaseProvider		m_db;
	AAAA::AAAAprovider		m_aaaa;
	prgbind::ProgramLibrary		m_prglib;
	proc::ProcessorProvider		m_proc;
};

} // namespace _Wolframe
#endif
