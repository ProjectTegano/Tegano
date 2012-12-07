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
///\file filter/typingfilter.hpp
///\brief Interface of filters that wrap untyped to typed input/output filters
#ifndef _Wolframe_TYPING_FILTER_HPP_INCLUDED
#define _Wolframe_TYPING_FILTER_HPP_INCLUDED
#include "filter/typedfilter.hpp"

namespace _Wolframe {
namespace langbind {

///\class TypingInputFilter
///\brief Typed fascade for input filter
class TypingInputFilter :public TypedInputFilter
{
public:
	///\brief Constructor
	explicit TypingInputFilter( const InputFilterR& inp)
		:m_inputfilter(inp){}

	///\brief Copy constructor
	///\param[in] o input filter to copy
	TypingInputFilter( const TypingInputFilter& o)
		:TypedInputFilter(o)
		,m_inputfilter(o.m_inputfilter){}

	///\brief Destructor
	virtual ~TypingInputFilter(){}

	///\brief Get a self copy
	///\return allocated pointer to copy of this
	virtual TypedInputFilter* copy() const		{return new TypingInputFilter(*this);}

	///\brief Implementation of TypedInputFilter::getNext(ElementType&,Element&)
	virtual bool getNext( ElementType& type, Element& element);

private:
	InputFilterR m_inputfilter;
};

///\class TypingOutputFilter
///\brief Typed fascade for output filter
class TypingOutputFilter :public TypedOutputFilter
{
public:
	///\brief Constructor
	explicit TypingOutputFilter( const OutputFilterR& outp)
		:m_outputfilter(outp){}

	///\brief Copy constructor
	///\param[in] o typed output filter to copy
	TypingOutputFilter( const TypingOutputFilter& o)
		:TypedOutputFilter(o)
		,m_outputfilter(o.m_outputfilter){}

	///\brief Destructor
	virtual ~TypingOutputFilter(){}

	///\brief Get a self copy
	///\return allocated pointer to copy of this
	virtual TypedOutputFilter* copy() const		{return new TypingOutputFilter(*this);}

	///\brief Implementation of TypedOutputFilter::print( ElementType,const Element&)
	virtual bool print( ElementType type, const Element& element);

private:
	OutputFilterR m_outputfilter;
};

}}//namespace
#endif


