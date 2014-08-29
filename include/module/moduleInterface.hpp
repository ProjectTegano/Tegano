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
/// \brief Basic interface classes that to build objects and the Wolframe module interface
/// \file module/moduleInterface.hpp

#ifndef _MODULE_INTERFACE_HPP_INCLUDED
#define _MODULE_INTERFACE_HPP_INCLUDED

#include <string>
#include <cstring>
#include <list>
#include <vector>
#include "config/configurationBase.hpp"
#include "module/constructor.hpp"

namespace _Wolframe {
namespace module {

/// \class BuilderBase
/// Base class of all builders
class BuilderBase
{
public:
	virtual ~BuilderBase()				{}

	virtual const char* objectClassName() const = 0;
	virtual ObjectConstructorBase::ObjectType objectType() const = 0;
	virtual ObjectConstructorBase* constructor() const = 0;
};

/// \class SimpleBuilder
/// \brief Base class for builders of objects without configuration
class SimpleBuilder
	:public BuilderBase
{
public:
	SimpleBuilder( const char* className_ )
		: m_className( className_ )		{}

	virtual ~SimpleBuilder()			{}

	virtual const char* objectClassName() const	{ return m_className; }

	virtual ObjectConstructorBase::ObjectType objectType() const = 0;
	virtual ObjectConstructorBase* constructor() const = 0;

	const char* className() const	{return m_className;}

private:
	const char* m_className;
};

/// \class ConfiguredBuilder
/// \brief Builder for objects with configuration
class ConfiguredBuilder
	:public BuilderBase
{
public:
	/// \brief Constructor
	/// \param title	string used for printing purposes, usually logging.
	/// \param section	configuration section (parent node)
	/// \param keyword	keyword in the configuration section. The object configuration
	///			is bind to the section, keyword pair
	/// \param className	the name of the class that the built constructor will build
	ConfiguredBuilder( const char* title_, const char* section_, const char* keyword_,
			   const char* className_ )
		: m_title( title_ ), m_section( section_ ), m_keyword( keyword_ ),
		  m_className( className_ )		{}

	virtual ~ConfiguredBuilder()			{}

	virtual const char* objectClassName() const	{ return m_className; }

	/// \brief Get the type of the object: filter, audit, command handler etc.
	/// This is not the same as the objectName
	virtual ObjectConstructorBase::ObjectType objectType() const = 0;

	/// \brief Get the configuration for the object
	/// \param logPrefix	string to print before the log messages generated inside this object.
	///			Same as for any confiuration object.
	virtual config::NamedConfiguration* configuration( const char* logPrefix ) const = 0;

	/// \brief get the virtual constructor for the object
	virtual ObjectConstructorBase* constructor() const = 0;

	const char* title() const	{return m_title;}
	const char* section() const	{return m_section;}
	const char* keyword() const	{return m_keyword;}
	const char* className() const	{return m_className;}

private:
	const char* m_title;		///< used for printing (logging etc.)
	const char* m_section;		///< configuration section to which the
					/// configuration parser reacts
	const char* m_keyword;		///< configuration keyword (element)
	const char* m_className;	///< class name of the object
};


/// \class ConfiguredBuilderDescription
/// \tparam Tconstructor
/// \tparam Tconf
/// \brief Template for constructing a configured builder.
template < class Tconstructor, class Tconf >
class ConfiguredBuilderDescription : public ConfiguredBuilder
{
public:
	ConfiguredBuilderDescription( const char* title_, const char* section_,
				      const char* keyword_, const char* className_ )
		: ConfiguredBuilder( title_, section_, keyword_, className_ )	{}

	virtual ~ConfiguredBuilderDescription()		{}

	virtual config::NamedConfiguration* configuration( const char* logPrefix ) const {
		return new Tconf( title(), logPrefix, keyword() );
	}
	virtual ObjectConstructorBase::ObjectType objectType() const	{
		return m_constructor.objectType();
	}
	virtual ObjectConstructorBase* constructor() const	{
		return new Tconstructor();
	}
private:
	Tconstructor m_constructor;
};


//*********** Module interface *********

/// \brief Function that constructs a builder.
///	This function is specific for each of the configured builders in the module.
typedef const BuilderBase* (*getBuilderFunc)();


/// \class ModuleEntryPoint
/// \brief The module entry point structure. Only one entry point per module.
struct ModuleEntryPoint
{
	enum	SignSize	{
		MODULE_SIGN_SIZE = 16
	};

	char signature[ MODULE_SIGN_SIZE];		///< module entry point signature
	unsigned short ifaceVersion;			///< version of the module loader interface
	const char* name;				///< name of the module
	getBuilderFunc* getBuilder;			///< NULL terminated array of functions that return the builders

public:
	ModuleEntryPoint( unsigned short iVer, const char* modName, getBuilderFunc* getBuilder_)
		: ifaceVersion( iVer ), name( modName ), getBuilder( getBuilder_ )
	{
		std::memset ( signature, 0, MODULE_SIGN_SIZE);
		std::memcpy ( signature, "Wolframe Module", std::strlen("Wolframe Module"));
	}
};

extern "C" ModuleEntryPoint entryPoint;

}} // namespace _Wolframe::module

#endif // _MODULE_INTERFACE_HPP_INCLUDED
