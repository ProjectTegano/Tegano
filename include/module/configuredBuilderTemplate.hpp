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
/// \brief Builder for objects with configuration defined by template parameter classes
/// \file module/configuredBuilderTemplate.hpp

#ifndef _MODULE_CONFIGURED_BUILDER_TEMPLATE_HPP_INCLUDED
#define _MODULE_CONFIGURED_BUILDER_TEMPLATE_HPP_INCLUDED
#include "module/configuredBuilder.hpp"

namespace _Wolframe {
namespace module {

/// \class ConfiguredBuilderTemplate
/// \tparam Tconstructor
/// \tparam Tconf
/// \brief Template for constructing a configured builder.
template < class Tconstructor, class Tconf >
class ConfiguredBuilderTemplate : public ConfiguredBuilder
{
public:
	ConfiguredBuilderTemplate( const char* title_, const char* section_,
				      const char* keyword_, const char* className_ )
		: ConfiguredBuilder( title_, section_, keyword_, className_ )	{}

	virtual ~ConfiguredBuilderTemplate()		{}

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

}}//namespace
#endif


