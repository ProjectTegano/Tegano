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
///\file appObjects.hpp
///\brief interface for application processor scripting language to system objects

#ifndef _Wolframe_langbind_APPOBJECTS_HPP_INCLUDED
#define _Wolframe_langbind_APPOBJECTS_HPP_INCLUDED
#include "protocol/outputfilter.hpp"
#include "protocol/inputfilter.hpp"
#include "ddl/structType.hpp"
#include <stack>
#include <string>

namespace _Wolframe {
namespace langbind {

///\class Output
///\brief Output as seen from scripting language
struct Output
{
	///\enum ItemType
	///\brief Output state
	enum ItemType
	{
		Data,		///< normal processing
		DoYield,	///< yield because rest of buffer not sufficient to complete operation
		Error		///< logic error in output. Operation is not possible
	};
	///\brief Constructor
	Output() :m_state(0){}
	///\brief Copy constructor
	///\param[in] o copied item
	Output( const Output& o) :m_outputfilter(o.m_outputfilter),m_state(0){}
	///\brief Destructor
	~Output(){}

	///\brief Print the next element
	///\param[in] e1 first element
	///\param[in] e1size first element size
	///\param[in] e2 second element
	///\param[in] e2size second element size
	///\return state returned
	ItemType print( const char* e1, unsigned int e1size, const char* e2, unsigned int e2size);

public:
	protocol::OutputFilterR m_outputfilter;	///< output filter reference

private:
	unsigned int m_state;						///< current state for outputs with more than one elements
};

///\class Input
///\brief input as seen from the application processor program
struct Input
{
	protocol::InputFilterR m_inputfilter;	///< input is defined by the associated input filter

	///\brief Constructor
	Input(){}
	///\brief Copy constructor
	///\param[in] o copied item
	Input( const Input& o) :m_inputfilter(o.m_inputfilter){}
	///\brief Destructor
	~Input(){}
};

///\class Filter
///\brief input/output filter as seen from the application processor program
struct Filter
{
	protocol::OutputFilterR m_outputfilter;		///< output filter
	protocol::InputFilterR m_inputfilter;		///< input filter

	///\brief Constructor
	///\param[in] name name of the filter as defined in the system
	Filter( const char* name);

	///\brief Default constructor
	Filter(){}

	///\brief Copy constructor
	///\param[in] o copied item
	Filter( const Filter& o)
		:m_outputfilter(o.m_outputfilter)
		,m_inputfilter(o.m_inputfilter){}
	///\brief Constructor
	///\param[in] fi input filter
	///\param[in] fo output filter
	Filter( const protocol::InputFilterR& fi, const protocol::OutputFilterR& fo)
		:m_outputfilter(fo)
		,m_inputfilter(fi){}
	///\brief Destructor
	~Filter(){}

	///\brief Get a member value of the filter
	///\param [in] name case sensitive name of the variable
	///\param [in] val the value returned
	///\return true on success, false, if the variable does not exist or the operation failed
	bool getValue( const char* name, std::string& val) const
	{
		if (m_inputfilter.get() && m_inputfilter->getValue( name, val)) return true;
		if (m_outputfilter.get() && m_outputfilter->getValue( name, val)) return true;
		return false;
	}

	///\brief Set a member value of the filter
	///\param [in] name case sensitive name of the variable
	///\param [in] value new value of the variable to set
	///\return true on success, false, if the variable does not exist or the operation failed
	bool setValue( const char* name, const std::string& value)
	{
		if (m_inputfilter.get() && m_inputfilter->setValue( name, value)) return true;
		if (m_outputfilter.get() && m_outputfilter->setValue( name, value)) return true;
		return false;
	}
};

struct Form
{
	ddl::StructTypeR m_struct;

	///\brief Default constructor
	Form() {}

	///\brief Copy constructor
	///\param[in] o copied item
	Form( const Form& o)
		:m_struct(o.m_struct){}
	///\brief Constructor
	///\param[in] st form data
	Form( const ddl::StructTypeR& st)
		:m_struct(st){}

	///\brief Destructor
	~Form(){}

	///\brief Get an atomic member value of the form
	///\param [in] name case sensitive name of the member
	///\param [in] val the value returned
	///\return true on success, false, if the member does not exist or is not atomic
	bool getValue( const char* name, std::string& val) const;

	///\brief Set an atomic member value of the form
	///\param [in] name case sensitive name of the member
	///\param [in] val new value of the member to set
	///\return true on success, false, if the member does not exist or is not atomic or the passed value is out of range
	bool setValue( const char* name, const std::string& val);

	///\brief Get an atomic member value of the form that is a vector
	///\param [in] idx index of the vector element
	///\param [in] val the value returned
	///\return true on success, false, if the member does not exist or is not atomic
	bool getValue( const std::size_t idx, std::string& val) const;

	///\brief Set an atomic member value of the form
	///\param [in] name case sensitive name of the member
	///\param [in] val new value of the member to set
	///\return true on success, false, if the member does not exist or is not atomic or the passed value is out of range
	bool setValue( const std::size_t idx, const std::string& val);
};

///\class InputFilterClosure
///\brief Closure for the input iterator (in Lua returned by 'input.get()')
class InputFilterClosure
{
public:
	///\enum ItemType
	///\brief Input loop state
	enum ItemType
	{
		EndOfData,	///< End of processed content reached
		Data,		///< normal processing, loop can continue
		DoYield,	///< have to yield and request more network input
		Error		///< have to stop processing because of an error
	};

	///\brief Constructor
	///\param[in] ig input filter reference from input
	InputFilterClosure( const protocol::InputFilterR& ig)
		:m_inputfilter(ig)
		,m_type(protocol::InputFilter::Value)
		,m_value(0)
		,m_buf(0)
		,m_bufsize(ig->getGenBufferSize())
		,m_bufpos(0)
		,m_taglevel(0)
	{
		m_buf = new char[ m_bufsize];
	}

	InputFilterClosure( const InputFilterClosure& o)
		:m_inputfilter(o.m_inputfilter)
		,m_type(o.m_type)
		,m_value(o.m_value)
		,m_buf(0)
		,m_bufsize(o.m_bufsize)
		,m_bufpos(o.m_bufpos)
		,m_taglevel(o.m_taglevel)
	{
		m_buf = new char[ m_bufsize];
		std::memcpy( m_buf, o.m_buf, m_bufpos);
	}

	///\brief Destructor
	~InputFilterClosure()
	{
		delete [] m_buf;
	}

	///\brief Internal buffer reset
	void init()
	{
		m_bufpos = 0;
		m_value = 0;
	}

	///\brief Get the next pair of elements
	///\param[out] e1 first element
	///\param[out] e1size first element size
	///\param[out] e2 second element
	///\param[out] e2size second element size
	///\return state returned
	ItemType fetch( const char*& e1, unsigned int& e1size, const char*& e2, unsigned int& e2size);

private:
	protocol::InputFilterR m_inputfilter;			///< rerefence to input with filter
	protocol::InputFilter::ElementType m_type;		///< current state (last value type parsed)
	char* m_value;						///< pointer to local copy of value in m_buf
	char* m_buf;						///< pointer to buffer for local copies of returned values
	std::size_t m_bufsize;					///< allocation size of m_buf
	std::size_t m_bufpos;					///< number of bytes filled in m_buf
	std::size_t m_taglevel;					///< current level in tag hierarchy
};

}}//namespace
#endif

