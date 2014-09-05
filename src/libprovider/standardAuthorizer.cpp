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
/// \brief Standard authorizer
#include "standardAuthorizer.hpp"
#include "aaaa/authorizationUnit.hpp"

using namespace _Wolframe;
using namespace _Wolframe::aaaa;

StandardAuthorizer::StandardAuthorizer( const std::vector< AuthorizationUnit* >& units,
					bool dflt )
	: m_authorizeUnits( units ), m_default( dflt )
{
}

StandardAuthorizer::~StandardAuthorizer()
{
}

void StandardAuthorizer::close()
{
}

// authorization requests
bool StandardAuthorizer::allowed( const Information& authzObject )
{
	bool retVal = true;
	bool ignored = true;

	for ( std::vector< AuthorizationUnit* >::const_iterator au = m_authorizeUnits.begin();
							au != m_authorizeUnits.end(); au++ )	{
		AuthorizationUnit::Result res = (*au)->allowed( authzObject );
		switch( res )	{
			case AuthorizationUnit::AUTHZ_ALLOWED:
				ignored = false;
				break;
			case AuthorizationUnit::AUTHZ_DENIED:
				ignored = false;
				retVal = false;
				break;
			case AuthorizationUnit::AUTHZ_IGNORED:
				break;
			case AuthorizationUnit::AUTHZ_ERROR:
				return false;
		}
	}
	if ( ignored )
		return m_default;
	else
		return retVal;
}
