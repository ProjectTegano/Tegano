/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
///\file serialize/mapContext.hpp
///\brief Defines the error handling of serialization/deserialization functions

#ifndef _Wolframe_SERIALIZE_STRUCT_MAPCONTEXT_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_MAPCONTEXT_HPP_INCLUDED
#include <string>
#include <stdexcept>
#include "filter/typedfilter.hpp"

namespace _Wolframe {
namespace serialize {

class SerializationErrorException :public std::runtime_error
{
public:
	SerializationErrorException( const char* title, const std::string& element, const std::string& tag);
	SerializationErrorException( const char* title, const std::string& tag);
	SerializationErrorException( const char* title);
};


class Context
{
public:
	struct ElementBuffer
	{
		langbind::FilterBase::ElementType m_type;
		langbind::TypedFilterBase::Element m_value;

		ElementBuffer()
			:m_type(langbind::FilterBase::Value)
			{}
		ElementBuffer( const ElementBuffer& o)
			:m_type(o.m_type)
			,m_value(o.m_value)
			{}
	};
	enum Flags
	{
		None=0x00,
		ValidateAttributes=0x01,
		SerializeWithIndices=0x02
	};
	static bool getFlag( const char* name, Flags& flg);

	Context( Flags f=None);
	Context( const Context& o);
	~Context(){}

	void clear();

	bool flag( Flags f) const					{return ((int)f & (int)m_flags) == (int)f;}
	void setFlags( Flags f)						{int ff=(int)m_flags | (int)f; m_flags=(Flags)ff;}

	void setElem( const ElementBuffer& e)
	{
		m_elem = e;
		m_has_elem = true;
	}

	void setElem( langbind::FilterBase::ElementType t)
	{
		m_elem.m_type = t;
		m_elem.m_value = langbind::TypedFilterBase::Element();
		m_has_elem = true;
	}

	void setElem( langbind::FilterBase::ElementType t, const char* str, std::size_t size)
	{
		m_elem.m_type = t;
		m_elem.m_value.value.string_.ptr = str;
		m_elem.m_value.value.string_.size = size;
		m_has_elem = true;
	}

	void setElem( langbind::FilterBase::ElementType t, langbind::TypedFilterBase::Element v)
	{
		m_elem.m_type = t;
		m_elem.m_value = v;
		m_has_elem = true;
	}

	bool getElem( ElementBuffer& e)
	{
		if (m_has_elem)
		{
			m_has_elem = false;
			e = m_elem;
			return true;
		}
		return false;
	}

private:
	Flags m_flags;
	ElementBuffer m_elem;
	bool m_has_elem;
};


}}
#endif

