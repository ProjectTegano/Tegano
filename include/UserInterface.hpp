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
///
// \file UserInterface.hpp
///

#ifndef _USER_INTERFACE_HPP_INCLUDED
#define _USER_INTERFACE_HPP_INCLUDED

#include <string>
#include <list>
#include "version.hpp"

namespace _Wolframe {
namespace UI {

/// User Interface object.
class InterfaceObject
{
public:
	/// User Interface object information.
	class Info
	{
	public:
		Info( const Info& o)
			: m_type( o.m_type ), m_platform( o.m_platform ),
			  m_name( o.m_name ), m_culture( o.m_culture ),
			  m_version( o.m_version ), m_description( o.m_description )	{}
		Info( const std::string& type_, const std::string& platform_,
		const std::string& name_, const std::string& culture_,
		      unsigned long version_, const std::string& description_ )
			: m_type( type_ ), m_platform( platform_ ),
			  m_name( name_ ), m_culture( culture_ ),
			  m_version( version_ ), m_description( description_ )	{}

		const std::string& type() const		{ return m_type; }
		const std::string& name() const		{ return m_name; }
		const std::string& platform() const	{ return m_platform; }
		const std::string& culture() const	{ return m_culture; }
		const Version& version() const		{ return m_version; }
		const std::string& description() const	{ return m_description; }
	private:
		const std::string	m_type;		///< the type of the object
		const std::string	m_platform;	///< platform for the object
		const std::string	m_name;		///< the name of the object
		const std::string	m_culture;	///< the culture of the object
		const Version		m_version;	///< the version of the object
		const std::string	m_description;	///< the description of the object
	};

	/// Constructor
	InterfaceObject( const InterfaceObject& o)
		: m_info( o.m_info ),
		  m_body ( o.m_body ){}
	InterfaceObject( const std::string& type_, const std::string& platform_,
			 const std::string& name_, const std::string& culture_,
			 unsigned long version_, const std::string& description_,
			 const std::string& body_ )
		: m_info( type_, platform_, name_, culture_, version_, description_ ),
		  m_body ( body_ )		{}

	const Info& info() const			{ return m_info; }
	const std::string& body() const			{ return m_body; }
private:
	const Info		m_info;			///< info for the object
	const std::string	m_body;			///< the body of the object
};


/// Library of user interface elements.
/// This is an interface (base class).
class UserInterfaceLibrary
{
public:
	virtual ~UserInterfaceLibrary()	{}

	// \brief Get a list of all object infos, latest versions
	// \param platform	the platform for which to get the object info
	// \param role		a role for which to get the object infos
	// \param culture	the culture for which to get the object info
	// \note  use an epmty string, "*" or "all" to get the object info
	///        for all platforms, roles, cultures ...
	virtual const std::list< InterfaceObject::Info > infos( const std::string& platform,
								const std::string& role,
								const std::string& culture ) const = 0;

	// \brief Get a list of all object infos, latest versions
	// \param platform	the platform for which to get the object info
	// \param roles	a list of roles for which to get the object infos
	// \param culture	the culture for which to get the object info
	virtual const std::list< InterfaceObject::Info > infos( const std::string& platform,
								const std::list< std::string >& roles,
								const std::string& culture ) const = 0;

	// \brief Get a list of object infos for all the versions of a object
	// \param platform	the platform for which to get the object info
	// \param name		the name of the object
	// \param culture	the culture for which to get the object info
	virtual const std::list< InterfaceObject::Info > versions( const std::string& platform,
								   const std::string& name,
								   const std::string& culture ) const = 0;

	// \brief Get the object
	// \param platform	the platform for which to get the object info
	// \param name		the name of the object
	// \param culture	the culture for which to get the object info
	virtual const InterfaceObject object( const std::string& platform,
					      const std::string& name,
					      const std::string& culture ) const = 0;

	// \brief Get the object
	// \param platform	the platform for which to get the object info
	// \param name		the name of the object
	// \param version	the version of the object
	// \param culture	the culture for which to get the object info
	virtual const InterfaceObject object( const std::string& platform,
					      const std::string& name, const Version& version,
					      const std::string& culture ) const = 0;

	// \brief Get the object
	// \param info		the object info
	virtual const InterfaceObject object( const InterfaceObject::Info& info ) const = 0;

	// \brief Add an object
	// \param object	the object to add
	virtual void addObject( const InterfaceObject& newObject ) const = 0;

	// \brief Delete an object
	// \param info		the object info
	virtual bool deleteObject( const InterfaceObject::Info& info ) const = 0;

	virtual void close()		{}
};

}} // namespace _Wolframe::UI

#endif // _USER_INTERFACE_HPP_INCLUDED
