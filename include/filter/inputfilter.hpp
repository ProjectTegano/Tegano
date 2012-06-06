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
///\file filter/inputfilter.hpp
///\brief Interface for input filter

#ifndef _Wolframe_FILTER_INPUTFILTER_INTERFACE_HPP_INCLUDED
#define _Wolframe_FILTER_INPUTFILTER_INTERFACE_HPP_INCLUDED
#include "countedReference.hpp"
#include "filter/filterbase.hpp"
#include <string>

namespace _Wolframe {
namespace langbind {

///\class InputFilter
///\brief Input filter
class InputFilter :public FilterBase
{
public:
	///\enum State
	///\brief State of the input filter
	enum State
	{
		Open,		//< serving data - normal input processing
		EndOfMessage,	//< have to yield processing because end of message reached
		Error		//< have to stop processing with an error
	};

	InputFilter()
		:m_state(Open){}

	virtual ~InputFilter(){}

	///\brief self copy
	///\return copy of this
	virtual InputFilter* copy() const=0;

	///\brief Declare the next input chunk to the filter
	///\param [in] ptr the start of the input chunk
	///\param [in] size the size of the input chunk in bytes
	///\param [in] end true, if end of input (the last chunk) has been reached.
	virtual void putInput( const void* ptr, std::size_t size, bool end)=0;

	///\brief Get the rest of the input chunk left unparsed yet (defaults to nothing left)
	///\param [out] ptr the start of the input chunk left to parse
	///\param [out] size the size of the input chunk left to parse in bytes
	///\param [out] end true, if end of input (the last chunk) has been reached.
	virtual void getRest( const void*& ptr, std::size_t& size, bool& end)
	{
		ptr = 0;
		size = 0;
		end = false;
	}

	///\brief Get next element
	///\param [out] type element type parsed
	///\param [out] element pointer to element returned
	///\param [out] elementsize size of the element returned
	///\return true, if success, false, if not.
	///\remark Check the state when false is returned
	virtual bool getNext( ElementType& type, const void*& element, std::size_t& elementsize)=0;

	///\brief Get the current state
	///\return the current state
	State state() const				{return m_state;}

	///\brief Set input filter state with error message
	///\param [in] s new state
	///\param [in] msg (optional) error to set
	void setState( State s, const char* msg=0)	{m_state=s; setError(msg);}

private:
	State m_state;				//< state
};

///\typedef InputFilterR
///\brief Shared input filter reference
typedef CountedReference<InputFilter> InputFilterR;


}}//namespace
#endif


