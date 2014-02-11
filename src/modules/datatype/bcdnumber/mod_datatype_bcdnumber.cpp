/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
///\file mod_datatype_datetime.cpp
///\brief Extension module for date and time arithmetic
#include "module/customDataTypeBuilder.hpp"
#include "logger-v1.hpp"
#include "datatypeBigint.hpp"
#include "datatypeBigfxp.hpp"

_Wolframe::log::LogBackend* logBackendPtr;

using namespace _Wolframe;
using namespace _Wolframe::module;

static void setModuleLogger( void* logger )
{
	logBackendPtr = reinterpret_cast< _Wolframe::log::LogBackend*>( logger);
}

static CustomDataTypeDef customDataTypes[] =
{
	{"bigint", &types::BigintDataType::create},
	{"bigfxp", &types::BigfxpDataType::create},
	{0,0}
};

namespace {
struct Obj
{
	static SimpleBuilder* constructor()
	{
		return new CustomDataTypeBuilder( "BcdArithmeticTypes", customDataTypes);
	}
};
}//anonymous namespace

enum {NofObjects=1};
static createBuilderFunc objdef[ NofObjects] =
{
	Obj::constructor
};

ModuleEntryPoint entryPoint( 0, "Extension module for arithmetics with arbitrary length BCD numbers", setModuleLogger, 0, 0, NofObjects, objdef);

