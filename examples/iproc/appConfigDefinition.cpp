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
// appConfigDefinition.cpp
//

#include "appConfig.hpp"
#include "standardConfigs.hpp"
#include "handlerConfig.hpp"

namespace _Wolframe {
namespace config {

ApplicationConfiguration::ApplicationConfiguration()
{
	// daemon / service configuration
	serviceCfg = new config::ServiceConfiguration();
	// network server
	serverCfg = new net::Configuration();
	// logging
	loggerCfg = new log::LoggerConfiguration();
	// application processor
	handlerCfg = new _Wolframe::HandlerConfiguration();

	// add sections, the parse function will select the
	// appropriate action
	addConfig( "service", serviceCfg );
	addConfig( "daemon", serviceCfg);

	addConfig( "listen", serverCfg );
	addConfig( "logging", loggerCfg );
	addConfig( "application", handlerCfg->m_appConfig );
}

ApplicationConfiguration::~ApplicationConfiguration()
{
	if (serviceCfg) delete serviceCfg;
	if (serverCfg) delete serverCfg;
	if (loggerCfg) delete loggerCfg;
	if (handlerCfg) delete handlerCfg;
}

}} // namespace _Wolframe::config

