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
/// \file module/baseObject.hpp
/// \brief Base class for objects loaded from module

#include "config/configurationObject.hpp"
#include "module/objectDescription.hpp"
#include <boost/shared_ptr.hpp>

#ifndef _Wolframe_MODULE_BASE_OBJECT_HPP_INCLUDED
#define _Wolframe_MODULE_BASE_OBJECT_HPP_INCLUDED

namespace _Wolframe {
namespace module {

/// \class BaseObject
/// \brief Base class for objects loaded from module
class BaseObject
{
public:
	virtual ~BaseObject(){}

	virtual void* rttiUpcast()=0;
};

/// \class BaseObjectInstance
/// \brief Implementation of the RTTI upcast method to get the real object instead of the base class
/// \note the idea is to implement the RTTI upcast in the module itself
template<class TYPE>
class BaseObjectInstance
	:public BaseObject
{
	virtual void* rttiUpcast()
	{
		return (void*)dynamic_cast<TYPE*>( this);
	}
};
}} // namespace
#endif
