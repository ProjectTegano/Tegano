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
///\file ddl/ddlStructSerializer.cpp

#include "serialize/ddl/ddlStructSerializer.hpp"
#include "filter/typedfilter.hpp"
#include "types/variant.hpp"
#include "types/variantStruct.hpp"
#include "types/variantStructDescription.hpp"
#include "utils/printFormats.hpp"
#include "serializationErrorException.hpp"
#include "logger/logger-v1.hpp"
#include <cstring>
#include <sstream>

using namespace _Wolframe;
using namespace serialize;

static std::string getElementPath( const DDLSerializeStateStack& stk)
{
	std::string rt;
	DDLSerializeStateStack::const_iterator itr=stk.begin(), end=stk.end();
	for (; itr != end; ++itr)
	{
		if (itr->value() && itr->value()->type() != types::VariantStruct::Array)
		{
			std::string tag = itr->tag().tostring();
			if (tag.size())
			{
				rt.append( "/");
				rt.append( tag);
			}
		}
	}
	return rt;
}

static bool fetchAtom( ElementBuffer& element, std::vector<DDLSerializeState>& stk)
{
	bool rt = false;
	if (stk.back().state())
	{
		stk.pop_back();
		return false;
	}
	if (stk.back().value()->initialized())
	{
		const types::Variant* val = stk.back().value();
		element.set( langbind::FilterBase::Value, *val);
		rt = true;
	}
	stk.back().state( 1);
	return rt;
}

static bool fetchStruct( ElementBuffer& element, std::vector<DDLSerializeState>& stk)
{
	bool rt = false;
	const types::VariantStruct* obj = (const types::VariantStruct*)stk.back().value();
	std::size_t idx = stk.back().state();
	if (idx < obj->nof_elements())
	{
		types::VariantStruct::const_iterator itr = obj->begin() + idx;
		types::VariantStructDescription::const_iterator di = obj->description()->begin() + idx;

		if (!itr->initialized())
		{
			stk.back().state( ++idx);
			return false;
		}
		if (di->attribute())
		{
			if (!*di->name)
			{
				throw SerializationErrorException( "error in structure definition: defined untagged value as attribute in structure", getElementPath( stk));
			}
			if (!itr->atomic())
			{
				throw SerializationErrorException( "named atomic value expected for attribute", getElementPath( stk));
			}
			types::VariantConst elem( di->name);
			element.set( langbind::FilterBase::Attribute, elem);
			rt = true;
			stk.back().state( ++idx);
			stk.push_back( DDLSerializeState( &*itr, elem));
		}
		else
		{
			if (!*di->name)
			{
				if (itr->type() == types::VariantStruct::Array)
				{
					throw SerializationErrorException( "non array element expected for content element", getElementPath( stk));
				}
				stk.back().state( ++idx);
				stk.push_back( DDLSerializeState( &*itr));
			}
			else if (itr->type() == types::VariantStruct::Array)
			{
				types::VariantConst elem( di->name);
				stk.back().state( ++idx);
				stk.push_back( DDLSerializeState( &*itr, elem));
			}
			else
			{
				types::VariantConst elem( di->name);
				element.set( langbind::FilterBase::OpenTag, elem);
				rt = true;
				stk.back().state( ++idx);
				stk.push_back( DDLSerializeState( langbind::FilterBase::CloseTag, elem));
				stk.push_back( DDLSerializeState( &*itr, elem));
			}
		}
	}
	else
	{
		stk.pop_back();
		if (stk.size() == 0)
		{
			element.set( langbind::FilterBase::CloseTag);
			rt = true;
		}
	}
	return rt;
}

static bool fetchVector( ElementBuffer& element, std::vector<DDLSerializeState>& stk)
{
	bool rt = false;
	const types::VariantStruct* obj = (const types::VariantStruct*)stk.back().value();
	std::size_t idx = stk.back().state();
	if (idx >= obj->nof_elements())
	{
		stk.pop_back();
		return false;
	}
	types::VariantStruct::const_iterator itr = obj->begin()+idx;
	bool hasTag = stk.back().tag().initialized();
	if (hasTag)
	{
		element.set( langbind::FilterBase::OpenTagArray, stk.back().tag());
		rt = true;
		stk.back().state( idx+1);
		stk.push_back( DDLSerializeState( langbind::FilterBase::CloseTag, stk.back().tag()));
		stk.push_back( DDLSerializeState( &*itr, stk.back().tag()));
	}
	else
	{
		stk.back().state( idx+1);
		stk.push_back( DDLSerializeState( &*itr, stk.back().tag()));
	}
	return rt;
}

static bool fetchObject( ElementBuffer& element, std::vector<DDLSerializeState>& stk)
{
	if (!stk.back().value())
	{
		element.set( stk.back().type(), stk.back().tag());
		stk.pop_back();
		return true;
	}
	else
	{
		switch (stk.back().value()->type())
		{
			case types::VariantStruct::Null:
			case types::VariantStruct::Bool:
			case types::VariantStruct::Double:
			case types::VariantStruct::Int:
			case types::VariantStruct::UInt:
			case types::VariantStruct::String:
			case types::VariantStruct::Custom:
			case types::VariantStruct::Timestamp:
			case types::VariantStruct::BigNumber:
			{
				return fetchAtom( element, stk);
			}
			case types::VariantStruct::Struct:
			{
				return fetchStruct( element, stk);
			}
			case types::VariantStruct::Indirection:
			{
				return false;
			}
			case types::VariantStruct::Unresolved:
			{
				return false;
			}
			case types::VariantStruct::Array:
			{
				return fetchVector( element, stk);
			}
		}
	}
	return false;
}


bool DDLStructSerializer::call()
{
	if (!m_out.get()) throw std::runtime_error( "no output for serialize");
	for (;;)
	{
		if (m_element.initialized())
		{
			if (!m_out->print( m_element.type(), m_element.value()))
			{
				if (m_out->getError())
				{
					throw SerializationErrorException( m_out->getError(), getElementPath( m_stk));
				}
				return false;
			}
			LOG_DATA << "[DDL structure serialization print] element " << langbind::InputFilter::elementTypeName( m_element.type()) << " '" << utils::getLogString( m_element.value()) << "'";
			m_element.markAsConsumed();
			continue;
		}
		fetchObject( m_element, m_stk);
		if (m_stk.empty()) return true;
		//REMARK: Check for stack empty after the last fetch, because the last close tag is not printed
	}
}


bool DDLStructSerializer::getNext( langbind::FilterBase::ElementType& type, types::VariantConst& value)
{
	for (;;)
	{
		if (m_element.initialized())
		{
			type = m_element.type();
			value = m_element.value();
			setState( langbind::InputFilter::Open);
			LOG_DATA << "[DDL structure serialization get] element " << langbind::InputFilter::elementTypeName( type) << " " << utils::getLogString( value);
			m_element.markAsConsumed();
			return true;
		}
		if (m_stk.empty()) return false;
		fetchObject( m_element, m_stk);
		//REMARK: The last close tag is returned (different to DDLStructSerializer::call())
	}
}

DDLStructSerializer::DDLStructSerializer( const types::VariantStruct* st)
	:langbind::TypedInputFilter("serializer")
	,m_st(st)
{
	if (!m_st) throw std::runtime_error( "empty form passed to serializer");
	m_stk.push_back( DDLSerializeState( st));
}

DDLStructSerializer::DDLStructSerializer( const DDLStructSerializer& o)
	:TypedInputFilter(o)
	,m_st(o.m_st)
	,m_element(o.m_element)
	,m_out(o.m_out)
	,m_stk(o.m_stk){}

DDLStructSerializer& DDLStructSerializer::operator =( const DDLStructSerializer& o)
{
	TypedInputFilter::operator=(o);
	m_st = o.m_st;
	m_element = o.m_element;
	m_out = o.m_out;
	m_stk = o.m_stk;
	return *this;
}

void DDLStructSerializer::init( const langbind::TypedOutputFilterR& out)
{
	m_element.markAsConsumed();
	m_stk.clear();
	m_stk.push_back( DDLSerializeState( m_st));
	m_out = out;
}

