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
/// \file appdevel/templates/cppFormFunctionTemplate.hpp
/// \brief Template for declaring C++ form function

#ifndef _Wolframe_appdevel_CPPFUNCTION_TEMPLATE_HPP_INCLUDED
#define _Wolframe_appdevel_CPPFUNCTION_TEMPLATE_HPP_INCLUDED

#include "serialize/cppFormFunction.hpp"
#include "processor/execContext.hpp"

namespace _Wolframe {
namespace appdevel {

/// \class CppFormFunction
/// \brief Application development template for form functions written in C++
template <class OutputType, class InputType, int FunctionType( proc::ExecContext* ctx, OutputType&, const InputType&)>
class CppFormFunctionTemplate
	:public serialize::CppFormFunction
{
public:
	CppFormFunctionTemplate( const std::string& name_)
		:serialize::CppFormFunction( name_, implementation, InputType::getStructDescription(), OutputType::getStructDescription())
	{}

private:
	static int implementation( proc::ExecContext* ctx, void* res, const void* param)
	{
		return FunctionType( ctx, *(OutputType*)res, *(const InputType*) param);
	}
};

}} //namespace
#endif



