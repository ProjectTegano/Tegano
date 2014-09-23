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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
/// \file serialize/validationFlags.hpp
/// \brief The flags defining the rules of validation

#ifndef _Wolframe_SERIALIZE_VALIDATION_FLAGS_HPP_INCLUDED
#define _Wolframe_SERIALIZE_VALIDATION_FLAGS_HPP_INCLUDED

namespace _Wolframe {
namespace serialize {

/// \struct ValidationFlags
/// \brief Set of flags that define the rules of validation (what is tolerated or not depends on what an input filter can provide and what strictness is imposed by the caller)
struct ValidationFlags
{
	/// \enum Enum
	/// \brief the enumeration that can also be interpreted as bit set
	enum Enum
	{
		None=0x00,				///< Empty flag set (no flags set)
		ValidateAttributes=0x01,		///< Do validate if attribute value pairs are serialized as such in the input filter serialization (and not as open content close) and the input filter does not declare itself as not knowing about attributes (PropagateNoAttr)
		ValidateInitialization=0x02,		///< Do validate if all declared elements in the structure were initilized by the input stream (if not explicitely declared as optional)
		ValidateArray=0x04,			///< Do validate if array elements are mapped to arrays and single elements not
		ValidateCase=0x08,			///< Do validate with case sensitive compare
		All=0x0F				///< All flags set (strict validation)
	};

	static bool match( ValidationFlags::Enum v, ValidationFlags::Enum e)
	{
		return ((int)v & (int)e) == (int)e;
	}

	static void unset( ValidationFlags::Enum& v, ValidationFlags::Enum f)
	{
		int n = (int)v - ((int)v & (int)f); 
		v = (ValidationFlags::Enum)n;
	}

	static void set( ValidationFlags::Enum& v, ValidationFlags::Enum f)
	{
		int p = (int)v | (int)f; 
		v = (ValidationFlags::Enum)p;
	}
};

}}//namespace
#endif
