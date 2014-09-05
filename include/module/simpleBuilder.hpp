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
/// \brief Base class for builders of objects without configuration
/// \file module/simpleBuilder.hpp

#ifndef _MODULE_SIMPLE_BUILDER_HPP_INCLUDED
#define _MODULE_SIMPLE_BUILDER_HPP_INCLUDED
#include "module/objectConstructorBase.hpp"
#include "module/builderBase.hpp"

namespace _Wolframe {
namespace module {

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

}}//namespace
#endif


