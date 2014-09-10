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
/// \brief Authorization provider
#ifndef _AAAA_AUTHORIZATION_PROVIDER_HPP_INCLUDED
#define _AAAA_AUTHORIZATION_PROVIDER_HPP_INCLUDED

#include "aaaa/authorizer.hpp"
#include "database/databaseProviderInterface.hpp"
#include "config/configurationObject.hpp"

#include <string>
#include <vector>

namespace _Wolframe {
namespace aaaa {

class AuthorizationUnit;
class StandardAuthorizer;

class AuthorizationProvider
{
public:
	AuthorizationProvider( const std::vector< config::ConfigurationObject* >& confs,
			       bool authzDefault,
			       const module::ModuleDirectory* modules );
	~AuthorizationProvider();
	bool resolveDB( const db::DatabaseProviderInterface& db );

	Authorizer* authorizer() const;

private:
	std::vector< AuthorizationUnit* >	m_authorizeUnits;
	StandardAuthorizer*			m_authorizer;
};

}}//namespace
#endif

