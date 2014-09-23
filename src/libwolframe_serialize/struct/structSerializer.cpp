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
///\file struct/structSerializer.cpp
///\brief Implements serialization
#include "serialize/struct/structSerializer.hpp"
#include "serialize/struct/serializeStack.hpp"
#include "serialize/elementBuffer.hpp"
#include "serializationErrorException.hpp"
#include "filter/typingfilter.hpp"
#include "utils/printFormats.hpp"
#include <cstring>
#include <stdexcept>
#include "logger/logger-v1.hpp"

using namespace _Wolframe;
using namespace serialize;

StructSerializer::StructSerializer( const ObjectReference& obj, const StructDescriptionBase* descr_)
	:TypedInputFilter("serializer")
	,m_ptr(obj.get())
	,m_obj(obj)
	,m_descr(descr_)
	,m_finalCloseFetched(false)
{
	if (m_descr)
	{
		m_stk.push_back( SerializeState( 0, m_descr->fetch(), m_ptr));
	}
}

StructSerializer::StructSerializer( const void* obj, const StructDescriptionBase* descr_)
	:TypedInputFilter("serializer")
	,m_ptr(obj)
	,m_descr(descr_)
	,m_finalCloseFetched(false)
{
	if (m_descr)
	{
		m_stk.push_back( SerializeState( 0, m_descr->fetch(), m_ptr));
	}
}

StructSerializer::StructSerializer( const StructSerializer& o)
	:TypedInputFilter(o)
	,m_ptr(o.m_ptr)
	,m_obj(o.m_obj)
	,m_descr(o.m_descr)
	,m_finalCloseFetched(o.m_finalCloseFetched)
	,m_element(o.m_element)
	,m_out(o.m_out)
	,m_stk(o.m_stk){}

void StructSerializer::init( const langbind::TypedOutputFilterR& out)
{
	m_element.markAsConsumed();
	m_stk.clear();
	m_stk.push_back( SerializeState( 0, m_descr->fetch(), m_ptr));
	m_finalCloseFetched = false;
	m_out = out;
}

void StructSerializer::reset()
{
	m_element.markAsConsumed();
	m_stk.clear();
	m_stk.push_back( SerializeState( 0, m_descr->fetch(), m_ptr));
	m_finalCloseFetched = false;
}

langbind::TypedInputFilter* StructSerializer::copy() const
{
	return new StructSerializer(*this);
}

std::string StructSerializer::getElementPath( const SerializeStateStack& stk)
{
	std::string rt;
	SerializeStateStack::const_iterator itr=stk.begin(), end=stk.end();
	for (; itr != end; ++itr)
	{
		if (itr->name())
		{
			rt.append( "/");
			rt.append( itr->name());
		}
	}
	return rt;
}

bool StructSerializer::call()
{
	if (!m_out.get()) throw std::runtime_error( "no output for serialize");
	while (m_stk.size())
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
			LOG_DATA << "[C++ structure serialization print] element " << langbind::InputFilter::elementTypeName( m_element.type()) << " '" << utils::getLogString( m_element.value()) << "'";
			m_element.markAsConsumed();
		}
		m_stk.back().fetch()( m_element, m_stk);
	}
	return true;
}

bool StructSerializer::getNext( langbind::FilterBase::ElementType& type, types::VariantConst& value)
{
	while (m_stk.size() && !m_element.initialized())
	{
		m_stk.back().fetch()( m_element, m_stk);
	}
	if (m_element.initialized())
	{
		type = m_element.type();
		value = m_element.value();
		setState( langbind::InputFilter::Open);
		LOG_DATA << "[C++ structure serialization get] element " << langbind::InputFilter::elementTypeName( m_element.type()) << " " << utils::getLogString( m_element.value());
		m_element.markAsConsumed();
		return true;
	}
	else
	{
		if (m_finalCloseFetched)
		{
			return false;
		}
		m_finalCloseFetched = true;
		type = langbind::FilterBase::CloseTag;
		value.init();
		LOG_DATA << "[C++ structure serialization get] final close";
		return true;
	}
}

