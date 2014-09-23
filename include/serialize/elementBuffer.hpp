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
/// \file serialize/elementBuffer.hpp
/// \brief Defines a buffer for one filter sequence element of serialization 

#ifndef _Wolframe_SERIALIZE_ELEMENT_BUFFER_HPP_INCLUDED
#define _Wolframe_SERIALIZE_ELEMENT_BUFFER_HPP_INCLUDED

namespace _Wolframe {
namespace serialize {

/// \class ElementBuffer
/// \brief Buffer for one element fetched
class ElementBuffer
{
public:
	ElementBuffer()
		:m_type(langbind::FilterBase::Value)
		,m_initialized(false)
		{}
	ElementBuffer( const ElementBuffer& o)
		:m_type(o.m_type)
		,m_value(o.m_value)
		,m_initialized(o.m_initialized)
		{}

	void set( langbind::FilterBase::ElementType t)
	{
		m_type = t;
		m_value.init();
		m_initialized = true;
	}

	void set( langbind::FilterBase::ElementType t, const types::VariantConst& v)
	{
		m_type = t;
		m_value = v;
		m_initialized = true;
	}

	void markAsConsumed()
	{
		m_initialized = false;
	}

	langbind::FilterBase::ElementType type() const	{return m_type;}
	const types::VariantConst& value() const	{return m_value;}
	bool initialized() const			{return m_initialized;}

private:
	langbind::FilterBase::ElementType m_type;
	types::VariantConst m_value;
	bool m_initialized;
};

}}//namespace
#endif

