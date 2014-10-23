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
/// \file module/objectConstructor.hpp
/// \brief Base class for a virtual constructor of a module object

#include "module/baseObject.hpp"
#include "module/objectDescription.hpp"
#include <boost/shared_ptr.hpp>

#ifndef _MODULE_OBJECT_CONSTRUCTOR_HPP_INCLUDED
#define _MODULE_OBJECT_CONSTRUCTOR_HPP_INCLUDED

namespace _Wolframe {
namespace module {

/// \brief Virtual constructor base class
class ObjectConstructor
	:public ObjectDescription
{
public:
	ObjectConstructor( const TypeId& typeId_, const std::string& className_ )
		:ObjectDescription( typeId_, className_ )
	{}
	ObjectConstructor( const TypeId& typeId_, const std::string& className_, const std::string& configSection_, const std::string& configKeyword_)
		:ObjectDescription( typeId_, className_, configSection_, configKeyword_)
	{}

	virtual ~ObjectConstructor(){}

	enum ObjectType
	{
		SimpleObjectType,
		ConfiguredObjectType
	};

	virtual ObjectType objectType() const=0;
	virtual const void* rttiUpcast() const=0;
};

}} // namespace
#endif
