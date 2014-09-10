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

/// \file aaaa/authorizationUnit.hpp
/// \brief Authorization unit

#ifndef _AUTHORIZATION_UNIT_HPP_INCLUDED
#define _AUTHORIZATION_UNIT_HPP_INCLUDED

#include <string>

#include "authorizer.hpp"
#include "database/databaseProviderInterface.hpp"
#include "module/objectDescription.hpp"

namespace _Wolframe {
namespace aaaa {

/// \class AuthorizationUnit
/// \brief This is the base class for authorization unit implementations
class AuthorizationUnit
{
public:
	explicit AuthorizationUnit( const std::string& id_)
		:m_id(id_){}

	enum Result	{
		AUTHZ_DENIED,
		AUTHZ_ALLOWED,
		AUTHZ_IGNORED,
		AUTHZ_ERROR
	};

	virtual ~AuthorizationUnit(){}

	virtual bool resolveDB( const db::DatabaseProviderInterface& /*db*/ )
						{ return true; }

	virtual Result allowed( const Information& ) = 0;

	const std::string& id() const
	{
		return m_id;
	}

private:
	std::string m_id;

private:
	void operator=( const AuthorizationUnit&){}
};

}} // namespace _Wolframe::aaaa

#endif // _AUTHORIZATION_HPP_INCLUDED
