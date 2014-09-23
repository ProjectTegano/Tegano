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
/// \file outputfilterImpl.hpp
/// \brief Output filter abstraction for the cjson library

#ifndef _Wolframe_CJSON_OUTPUT_FILTER_HPP_INCLUDED
#define _Wolframe_CJSON_OUTPUT_FILTER_HPP_INCLUDED
#include "filter/outputfilter.hpp"
#include "types/docmetadata.hpp"
#include "types/string.hpp"
extern "C"
{
#include "cJSON.h"
}
#include <cstdlib>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

class OutputFilterImpl
	:public OutputFilter
{
public:
	typedef OutputFilter Parent;

	explicit OutputFilterImpl( const types::DocMetaDataR& inheritedMetaData)
		:OutputFilter("cjson", inheritedMetaData)
		,m_elemitr(0)
		,m_headerprinted(false)
		,m_lastelemtype(FilterBase::OpenTag)
	{
		m_stk.push_back( StackElement(""));
	}

	OutputFilterImpl( const OutputFilterImpl& o)
		:OutputFilter(o)
		,m_attribname(o.m_attribname)
		,m_elembuf(o.m_elembuf)
		,m_elemitr(o.m_elemitr)
		,m_encattr(o.m_encattr)
		,m_headerprinted(o.m_headerprinted)
		,m_stk(o.m_stk)
		,m_lastelemtype(o.m_lastelemtype)
		{}

	virtual ~OutputFilterImpl(){}

	/// \brief Implementation of OutputFilter::copy()
	virtual OutputFilterImpl* copy() const
	{
		return new OutputFilterImpl( *this);
	}

	/// \brief Implementation of OutputFilter::print( ElementType, const void*,std::size_t)
	virtual bool print( ElementType type, const void* element, std::size_t elementsize);

	/// \brief Implementation of OutputFilter::getOutput( const void*&,std::size_t&)
	virtual void getOutput( const void*& buf, std::size_t& bufsize);

	/// \brief Implementation of OutputFilter::close()
	virtual bool close();

private:
	void printHeader();
	void addStructValue( const std::string name, const std::string& value, bool asArray);
	void addStructItem( const std::string name, cJSON* val, bool asArray);
	void setContentValue( const std::string& value);
	void closeElement();
	void printStructToBuffer();
	std::string elementpath() const;

private:
	std::string m_attribname;				///< attribute name buffer
	std::string m_elembuf;					///< buffer for current element
	std::size_t m_elemitr;					///< iterator on current element
	types::String::EncodingAttrib m_encattr;		///< character set encoding attributes
	bool m_headerprinted;					///< true if the header has already been printed

	struct StackElement
	{
		explicit StackElement( const std::string& name_, bool array_=false)
			:m_state(Init),m_node(0),m_name(name_),m_array(array_){}
		StackElement( const StackElement& o)
			:m_state(o.m_state),m_node(o.m_node?cJSON_Duplicate(o.m_node,1):0),m_name(o.m_name),m_array(o.m_array){}
		~StackElement()
		{
			if (m_node) cJSON_Delete( m_node);
		}

		enum State {Init};
		State m_state;
		cJSON* m_node;
		std::string m_name;
		bool m_array;
	};
	std::vector<StackElement> m_stk;
	FilterBase::ElementType m_lastelemtype;			///< last element type
};

}}//namespace
#endif

