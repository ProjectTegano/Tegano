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
///\file module.hpp
///\brief module object base class for implementing modules in C++

#ifndef MODULE_HPP_INCLUDED
#define MODULE_HPP_INCLUDED

namespace _Wolframe {
namespace module {

struct Module
{
	struct Object
	{
		virtual ~Object() {}
		virtual const char* type() const=0;
		virtual const char* name() const=0;
		virtual const std::string description() const=0;
	};

	Module()
	{
		mod.name = "";
		mod.description = "";
		mod.destroy = &destroy;
		mod.nof_objects = 0;
		mod.objects = 0;
	}

	template <class OBJ>
	boost::enable_if< boost::is_base_of<Object, OBJ>, Module&>::Type operator()( const Object& objref)
	//... only available if OBJ is derived from Object
	{
		mod.objects = realloc( objst.objects, sizeof(*objst.objects)*(objst.nof_objects+1));
		mod.type = type();
		mod.name = name();
		mod.description = description();
		mod.reference = (void*)&objref;
	}

	const wolframe_ModuleStruct* getReference() const {return &mod;}

private:
	wolframe_ModuleStruct mod;
	static void destroy( wolframe_ModuleStruct* m)
	{
		free( m.objects);
		m.objects = 0;
	}
};

}} //namespace

#endif //MODULE_HPP_INCLUDED

