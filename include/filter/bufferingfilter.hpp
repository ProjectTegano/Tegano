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
///\file filter/bufferingfilter.hpp
///\brief Interface for input filter

#ifndef _Wolframe_FILTER_BUFFERING_FILTER_INTERFACE_HPP_INCLUDED
#define _Wolframe_FILTER_BUFFERING_FILTER_INTERFACE_HPP_INCLUDED
#include "types/countedReference.hpp"
#include "filter/inputfilter.hpp"
#include <string>

namespace _Wolframe {
namespace langbind {

///\class BufferingInputFilter
///\brief Input filter that buffers the input before redirecting to InputFilter::getNext(ElementType&,const void*&,std::size_t&)
class BufferingInputFilter :public InputFilter
{
public:
	BufferingInputFilter( InputFilter* ref)
		:types::TypeSignature("langbind::BufferingInputFilter", __LINE__)
		,m_ref(ref->copy()),m_end(false){}

	BufferingInputFilter( const BufferingInputFilter& o)
		:types::TypeSignature(o)
		,InputFilter(o)
		,m_ref(o.m_ref->copy())
		,m_end(o.m_end){}

	virtual ~BufferingInputFilter()
	{
		delete m_ref;
	}

	///\brief Implements InputFilter::copy()const
	virtual InputFilter* copy() const
	{
		return new BufferingInputFilter( m_ref);
	}

	///\brief Implements InputFilter::putInput(const void*,std::size_t,bool)
	virtual void putInput( const void* ptr, std::size_t size, bool end_)
	{
		m_buf.append( (const char*)ptr, size);
		if (end_)
		{
			m_ref->putInput( m_buf.c_str(), m_buf.size(), m_end=true);
		}
	}

	///\brief Implements InputFilter::getNext(ElementType&,const void*&,std::size_t&)
	virtual bool getNext( ElementType& type, const void*& element, std::size_t& elementsize)
	{
		if (m_end)
		{
			bool rt = m_ref->getNext( type, element, elementsize);
			setState( m_ref->state(), m_ref->getError());
			return rt;
		}
		else
		{
			setState( EndOfMessage);
			return false;
		}
	}

	virtual bool getDocType( std::string& val)
	{
		if (m_end)
		{
			bool rt = m_ref->getDocType( val);
			setState( m_ref->state(), m_ref->getError());
			return rt;
		}
		else
		{
			setState( EndOfMessage);
			return false;
		}
	}

	///\brief Implements FilterBase::getValue(const char*,std::string&)
	virtual bool getValue( const char* name, std::string& val) const
	{
		return m_ref->getValue( name, val);
	}

	///\brief Implements FilterBase::setValue(const char*,const std::string&)
	virtual bool setValue( const char* name, const std::string& val)
	{
		return m_ref->setValue( name, val);
	}

	InputFilter* reference() const
	{
		return m_ref;
	}

	bool end() const
	{
		return m_end;
	}
private:
	InputFilter* m_ref;
	std::string m_buf;
	bool m_end;
};

}}//namespace
#endif


