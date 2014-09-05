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
/// \file processor/execContext.hpp
/// \brief TEST REPLACEMENT OF execution context
#ifndef _WOLFRAME_PROCESSOR_EXEC_CONTEXT_HPP_INCLUDED
#define _WOLFRAME_PROCESSOR_EXEC_CONTEXT_HPP_INCLUDED
#include "processor/procProviderInterface.hpp"

namespace _Wolframe {

namespace aaaa {
class User {public: User(){} User(const User&){}};
class Authorizer {public: Authorizer(){} Authorizer(const Authorizer&){}};
}
namespace proc {

/// \class ExecContext
/// \brief See include/processor/execContext.hpp
class ExecContext
{
public:
	/// \brief Default Constructor
	ExecContext()
		:m_provider(0){}
	/// \brief Constructor
	explicit ExecContext( const ProcessorProviderInterface* p)
		:m_provider(p){}
	/// \brief Copy constructor
	ExecContext( const ExecContext& o)
		:m_provider(o.m_provider),m_user(o.m_user),m_authorizer(o.m_authorizer){}

	/// \brief Get the processor provider interface
	const ProcessorProviderInterface* provider() const	{return m_provider;}

	/// \brief Get the user data
	const aaaa::User& user() const				{return m_user;}
	/// \brief Set the user data
	void setUser( const aaaa::User& u)			{m_user = u;}

	/// \brief Get the authorization instance interface
	const aaaa::Authorizer* authorizer()			{return m_authorizer;}
	/// \brief Set the authorization instance interface
	void setAuthorizer( const aaaa::Authorizer* a)		{m_authorizer = a;}

	/// \brief Checks if a function tagged with AUTHORIZE( authorizationFunction, authorizationResource) is allowed to be executed
	bool checkAuthorization( const std::string& /*authorizationFunction*/, const std::string& /*authorizationResource*/)
	{
		return true;
	}

public:
	const ProcessorProviderInterface* m_provider;		///< processor provider interface
	aaaa::User m_user;					///< user data
	const aaaa::Authorizer* m_authorizer;			///< instance to query for execution permission based on login data
};

}} //namespace
#endif

