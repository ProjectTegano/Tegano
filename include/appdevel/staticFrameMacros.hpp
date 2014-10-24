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
/// \file appdevel/staticFrameMacros.hpp
/// \brief Macros for defining the frame of a Wolframe application extension module
#include "module/baseObject.hpp"
#include "module/objectConstructor.hpp"
#include <stdexcept>

/// \brief Defines a static object as container of objects declared as in a module
#define WF_STATIC_FRAME_BEGIN(NAME)\
	namespace {\
	struct Type ## NAME\
	{\
		enum {MaxNofBuilders=64};\
		_Wolframe::module::GetObjectConstructorFunc ar[ MaxNofBuilders];\
		std::size_t size;\
		Type ## NAME()\
			:size(0)\
		{\
			ar[0] = 0;\
		}\
		Type ## NAME operator()( _Wolframe::module::GetObjectConstructorFunc func)\
		{\
			if (size +1 >= MaxNofBuilders) throw std::logic_error("too many builder objects defined in module '" #NAME "' (maximum of 64 objects)");\
			ar[ size] = func;\
			ar[ size+1] = 0;\
			size += 1;\
			return *this;\
		}\
		const _Wolframe::module::ObjectConstructor& operator[]( std::size_t idx) const\
		{\
			if (idx >= size) throw std::logic_error("array bound read");\
			return *ar[ idx]();\
		}\
	};\
	struct NAME :public Type ## NAME\
	{\
		NAME()\
		{

#define WF_STATIC_FRAME_END\
		}\
	};\
	}


