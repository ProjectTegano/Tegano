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
/// \file module/simpleObjectConstructor.hpp
/// \brief Base class for virtual constructors of objects without configuration

#include "module/objectConstructor.hpp"
#include <boost/shared_ptr.hpp>

#ifndef _Wolframe_SIMPLE_OBJECT_CONSTRUCTOR_HPP_INCLUDED
#define _Wolframe_SIMPLE_OBJECT_CONSTRUCTOR_HPP_INCLUDED

namespace _Wolframe {
namespace module {

/// \brief Constructor of a simple (without configuration) object
class SimpleObjectConstructor
	:public ObjectConstructor
{
public:
	SimpleObjectConstructor( const TypeId& typeId_, const std::string& className_ )
		:ObjectConstructor( typeId_, className_ ){}

	virtual ~SimpleObjectConstructor(){}

	virtual BaseObject* object() const = 0;

	virtual ObjectType objectType() const
	{
		return SimpleObjectType;
	}
	virtual const void* rttiUpcast() const
	{
		return (const void*)this;
	}
};

}}// namespac
#endif
