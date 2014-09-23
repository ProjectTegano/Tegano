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
///\file configSerialize.cpp
///\brief Defines functions for serialization/deserialization of configuration

#include "serialize/configSerialize.hpp"
#include "utils/printFormats.hpp"
#include "utils/stringUtils.hpp"

using namespace _Wolframe;
using namespace _Wolframe::serialize;

void serialize::parseConfigStructureVP( void* stptr, const serialize::StructDescriptionBase* descr, const config::ConfigurationNode& pt)
{
	langbind::PropertyTreeInputFilter* filter;
	langbind::TypedInputFilterR inp( filter=new langbind::PropertyTreeInputFilter( pt));
	serialize::StructParser parser( stptr, descr);
	serialize::ValidationFlags::Enum vflags = serialize::ValidationFlags::All;
	serialize::ValidationFlags::unset( vflags, serialize::ValidationFlags::ValidateCase);
	serialize::ValidationFlags::unset( vflags, serialize::ValidationFlags::ValidateArray);
	parser.init( inp, vflags);
	try
	{
		if (!parser.call()) throw std::runtime_error( "illegal state in structure parser");
	}
	catch (const std::bad_alloc& e)
	{
		throw e;
	}
	catch (const std::runtime_error& e)
	{
		throw std::runtime_error( std::string("configuration error ") + filter->posLogText() + ": " + e.what());
	}
}

std::string serialize::structureToStringVP( const void* stptr, const serialize::StructDescriptionBase* descr)
{
	serialize::StructSerializer serializer( stptr, descr);
	return utils::filterToString( serializer, utils::ptreePrintFormat());
}


