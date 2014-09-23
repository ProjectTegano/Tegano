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
/// \file inputfilterImpl.hpp
/// \brief Input filter abstraction for the cJSON library

#ifndef _Wolframe_CJSON_INPUT_FILTER_HPP_INCLUDED
#define _Wolframe_CJSON_INPUT_FILTER_HPP_INCLUDED
#include "filter/inputfilter.hpp"
#include "types/string.hpp"
#include "types/docmetadata.hpp"
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

struct InputFilterImpl :public InputFilter
{
	typedef InputFilter Parent;

	InputFilterImpl()
		:InputFilter("cjson")
		,m_firstnode(0)
		,m_buffered_element(0)
		,m_buffered_element_size(0)
		,m_done(false)
	{}

	InputFilterImpl( const InputFilterImpl& o)
		:InputFilter(o)
		,m_content(o.m_content)
		,m_encattr(o.m_encattr)
		,m_root(o.m_root)
		,m_firstnode(o.m_firstnode)
		,m_stk(o.m_stk)
		,m_buffered_element(o.m_buffered_element)
		,m_buffered_element_size(o.m_buffered_element_size)
		,m_done(o.m_done)
	{}

	virtual ~InputFilterImpl(){}

	/// \brief Implements InputFilter::copy()
	virtual InputFilter* copy() const
	{
		return new InputFilterImpl(*this);
	}

	/// \brief Implements InputFilter::putInput(const void*,std::size_t,bool)
	virtual void putInput( const void* content, std::size_t contentsize, bool end);

	/// \brief Implements InputFilter::getMetaData()
	virtual const types::DocMetaData* getMetaData();

	/// \brief implement interface member InputFilter::getNext( typename FilterBase::ElementType&,const void*&,std::size_t&)
	virtual bool getNext( InputFilter::ElementType& type, const void*& element, std::size_t& elementsize);

private:
	boost::shared_ptr<cJSON> parse( const std::string& content);
	bool getNodeData( const cJSON* nd, const char*& eename, InputFilter::ElementType& eetype, const void*& ee, std::size_t& eesize) const;
	
	std::string stackdump() const;

private:
	std::string m_content;
	types::String::EncodingAttrib m_encattr;	///< character set encoding attributes
	boost::shared_ptr<cJSON> m_root;		///< data structure holding the whole tree
	const cJSON* m_firstnode;			///< first node (to detect if getNext has been called)

	struct StackElement
	{
		explicit StackElement( const cJSON* node_, const char* tag_=0)
			:m_state(StateValue),m_node(node_),m_tag(tag_),m_visible(true){}
		StackElement( const StackElement& o)
			:m_state(o.m_state),m_node(o.m_node),m_tag(o.m_tag),m_visible(o.m_visible){}

		enum State {StateValue,StateAttributeValue,StateContentValue,StateContentClose,StateNext,StateReopenArray};
		static const char* stateName( State i)
		{
			static const char* ar[] = {"StateValue","StateAttributeValue","StateContentValue","StateContentClose","StateNext","StateReopenArray"};
			return ar[i];
		}
		State m_state;				///< current state
		const cJSON* m_node;			///< current node
		const char* m_tag;			///< current tag name
		bool m_visible;				///< do issue CloseTag after discarding
	};
	std::vector<StackElement> m_stk;		///< state stack
	const void* m_buffered_element;			///< pointer to content element
	std::size_t m_buffered_element_size;		///< size of content element
	bool m_done;					///< true, when final close has been returned
};

}}//namespace
#endif
