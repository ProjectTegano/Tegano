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
/// \brief Entry point as pointer to structure, that is the root of all data structures and information exported by a module
/// \file module/moduleEntryPoint.hpp
#ifndef _MODULE_ENTRY_POINT_HPP_INCLUDED
#define _MODULE_ENTRY_POINT_HPP_INCLUDED
#include <cstring>
#include "module/objectConstructor.hpp"

namespace _Wolframe {
namespace module {

/// \brief Function type to get an object constructor.
typedef const ObjectConstructor* (*GetObjectConstructorFunc)();

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
	GetObjectConstructorFunc* constructors;		///< NULL terminated array of functions that return the object constructors

public:
	ModuleEntryPoint( unsigned short iVer, const char* modName, GetObjectConstructorFunc* constructors_)
		: ifaceVersion( iVer ), name( modName ), constructors( constructors_ )
	{
		std::memset ( signature, 0, MODULE_SIGN_SIZE);
		std::memcpy ( signature, "Wolframe Module", std::strlen("Wolframe Module"));
	}
};

#ifndef _WIN32
#define DLLEXPORT 
#else
#define DLLEXPORT __declspec( dllexport )
#endif

extern "C" DLLEXPORT ModuleEntryPoint entryPoint;

}}//namespace
#endif
