/************************************************************************
Copyright (C) 2011, 2012 Project Wolframe.
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
///\file serialize_struct_filtermapSerialize.cpp
///\brief Helper function for the serialization of structures
#include "serialize/struct/filtermapSerialize.hpp"
#include "serialize/struct/filtermapPrintValue.hpp"

using namespace _Wolframe;
using namespace _Wolframe::serialize;

bool _Wolframe::serialize::printValue_int( const signed int* ptr, langbind::TypedFilterBase::Element& value)
{
	return traits::printValue( *(const int*)ptr, value);
}

bool _Wolframe::serialize::printValue_uint( const unsigned int* ptr, langbind::TypedFilterBase::Element& value)
{
	return traits::printValue( *(const unsigned int*)ptr, value);
}

bool _Wolframe::serialize::printValue_ulong( const unsigned long* ptr, langbind::TypedFilterBase::Element& value)
{
	return traits::printValue( *(const unsigned long*)ptr, value);
}

bool _Wolframe::serialize::printValue_long( const signed long* ptr, langbind::TypedFilterBase::Element& value)
{
	return traits::printValue( *(const unsigned long*)ptr, value);
}

bool _Wolframe::serialize::printValue_short( const signed short* ptr, langbind::TypedFilterBase::Element& value)
{
	return traits::printValue( *(const signed short*)ptr, value);
}

bool _Wolframe::serialize::printValue_ushort( const unsigned short* ptr, langbind::TypedFilterBase::Element& value)
{
	return traits::printValue( *(const unsigned short*)ptr, value);
}

bool _Wolframe::serialize::printValue_char( const signed char* ptr, langbind::TypedFilterBase::Element& value)
{
	return traits::printValue( *(const signed char*)ptr, value);
}

bool _Wolframe::serialize::printValue_uchar( const unsigned char* ptr, langbind::TypedFilterBase::Element& value)
{
	return traits::printValue( *(const unsigned char*)ptr, value);
}

bool _Wolframe::serialize::printValue_float( const float* ptr, langbind::TypedFilterBase::Element& value)
{
	return traits::printValue( *(const float*)ptr, value);
}

bool _Wolframe::serialize::printValue_double( const double* ptr, langbind::TypedFilterBase::Element& value)
{
	return traits::printValue( *(const double*)ptr, value);
}

bool _Wolframe::serialize::printValue_string( const std::string* ptr, langbind::TypedFilterBase::Element& value)
{
	return traits::printValue( *(const std::string*)ptr, value);
}

bool _Wolframe::serialize::fetchCloseTag( Context& ctx, FiltermapSerializeStateStack& stk)
{
	ctx.setElem(langbind::FilterBase::CloseTag);
	stk.pop_back();
	return true;
}

bool _Wolframe::serialize::fetchOpenTag( Context& ctx, FiltermapSerializeStateStack& stk)
{
	ctx.setElem(
		langbind::FilterBase::OpenTag,
		langbind::TypedFilterBase::Element( stk.back().name()));
	stk.pop_back();
	return true;
}

bool _Wolframe::serialize::fetchObjectStruct( const StructDescriptionBase* descr, Context& ctx, FiltermapSerializeStateStack& stk)
{
	bool rt = false;
	const void* obj = stk.back().value();
	std::size_t idx = stk.back().state();
	if (idx < descr->nof_elements())
	{
		StructDescriptionBase::Map::const_iterator itr = descr->begin() + idx;

		if (idx < descr->nof_attributes())
		{
			if (itr->second.type() != StructDescriptionBase::Atomic)
			{
				throw SerializationErrorException( "atomic value expected for attribute", StructSerializer::getElementPath( stk));
			}
			ctx.setElem(
				langbind::FilterBase::Attribute,
				langbind::TypedFilterBase::Element( itr->first.c_str(), itr->first.size()));
			rt = true;
			stk.back().state( idx+1);
			stk.push_back( FiltermapSerializeState( itr->first.c_str(), itr->second.fetch(), (char*)obj + itr->second.ofs()));
		}
		else
		{
			ctx.setElem(
				langbind::FilterBase::OpenTag,
				langbind::TypedFilterBase::Element( itr->first.c_str(), itr->first.size()));
			rt = true;
			stk.back().state( idx+1);
			stk.push_back( FiltermapSerializeState( 0, &fetchCloseTag, itr->first.c_str()));
			stk.push_back( FiltermapSerializeState( itr->first.c_str(), itr->second.fetch(), (char*)obj + itr->second.ofs()));
		}
	}
	else
	{
		stk.pop_back();
		if (stk.size() == 0)
		{
			ctx.setElem( langbind::FilterBase::CloseTag, langbind::TypedFilterBase::Element());
			rt = true;
		}
	}
	return rt;
}

bool _Wolframe::serialize::fetchObjectAtomic( PrintValue prnt, Context& ctx, FiltermapSerializeStateStack& stk)
{
	Context::ElementBuffer elem;
	elem.m_type = langbind::FilterBase::Value;
	if (!prnt( stk.back().value(), elem.m_value))
	{
		throw SerializationErrorException( "atomic value conversion error", StructSerializer::getElementPath( stk));
	}
	ctx.setElem( elem);
	stk.pop_back();
	return true;
}

bool _Wolframe::serialize::fetchObjectVectorElement( FetchElement fetchElement, const void* ve, Context& ctx, FiltermapSerializeStateStack& stk)
{
	bool rt = false;
	std::size_t idx = stk.back().state();

	if (ctx.flag( Context::SerializeWithIndices))
	{
		ctx.setElem( langbind::FilterBase::OpenTag, langbind::TypedFilterBase::Element( (unsigned int)idx+1));
		stk.back().state( idx+1);
		stk.push_back( FiltermapSerializeState( 0, &fetchCloseTag, 0));
		stk.push_back( FiltermapSerializeState( stk.back().name(), fetchElement, ve));
	}
	else
	{
		const char* tagname = (const char*)stk.at( stk.size()-2).value();
		if (idx >= 1)
		{
			ctx.setElem( langbind::FilterBase::CloseTag);
			rt = true;
		}
		stk.back().state( idx+1);
		stk.push_back( FiltermapSerializeState( stk.back().name(), fetchElement, ve));
		if (idx >= 1)
		{
			stk.push_back( FiltermapSerializeState( tagname, &fetchOpenTag, tagname));
		}
	}
	return rt;
}

