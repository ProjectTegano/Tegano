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
/// \file filter/typedfilter.hpp
/// \brief Typed interface for input/output filter

#ifndef _Wolframe_FILTER_TYPEDFILTER_INTERFACE_HPP_INCLUDED
#define _Wolframe_FILTER_TYPEDFILTER_INTERFACE_HPP_INCLUDED
#include "types/variant.hpp"
#include "filter/filterbase.hpp"
#include "filter/inputfilter.hpp"
#include "filter/outputfilter.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

/// \class TypedInputFilter
/// \brief Input filter with atomic values having a type
class TypedInputFilter
	:public FilterBase
{
public:
	typedef InputFilter::State State;

	/// \brief Constructor
	explicit TypedInputFilter( const char* name_)
		:FilterBase(name_)
		,m_state(InputFilter::Start){}

	/// \brief Copy constructor
	/// \param[in] o typed output filter to copy
	TypedInputFilter( const TypedInputFilter& o)
		:FilterBase(o)
		,m_state(o.m_state){}

	/// \brief Destructor
	virtual ~TypedInputFilter(){}

	/// \brief Get a self copy
	/// \return allocated pointer to copy of this
	virtual TypedInputFilter* copy() const=0;

	/// \brief Get next element
	/// \param [out] type element type parsed
	/// \param [out] element reference to element returned
	/// \return true, if success, false, if not.
	/// \remark Check the state when false is returned
	virtual bool getNext( ElementType& type, types::VariantConst& element)=0;

	/// \brief Get the current state
	/// \return the current state
	State state() const				{return m_state;}

	/// \brief Set input filter state with error message
	/// \param [in] s new state
	/// \param [in] msg (optional) error to set
	void setState( State s, const char* msg=0)	{m_state=s; setError(msg);}

	/// \brief Set the iterator to the start (if implemented)
	virtual void resetIterator(){}

private:
	State m_state;					///< state
};

/// \typedef TypedInputFilterR
/// \brief Shared input filter (langbind::TypedInputFilter) reference
typedef boost::shared_ptr<TypedInputFilter> TypedInputFilterR;



/// \class TypedOutputFilter
/// \brief Output filter with atomic values having a type
class TypedOutputFilter :public FilterBase
{
public:
	typedef OutputFilter::State State;

	/// \brief Constructor
	explicit TypedOutputFilter( const char* name_)
		:FilterBase(name_)
		,m_state(OutputFilter::Start) {}

	/// \brief Copy constructor
	/// \param[in] o typed output filter to copy
	TypedOutputFilter( const TypedOutputFilter& o)
		:FilterBase(o)
		,m_state(o.m_state){}

	/// \brief Destructor
	virtual ~TypedOutputFilter(){}

	/// \brief Get a self copy
	/// \return allocated pointer to copy of this
	virtual TypedOutputFilter* copy() const=0;

	/// \brief Get next element
	/// \param [out] type element type parsed
	/// \param [out] element reference to element returned
	/// \return true, if success, false, if not.
	/// \remark Check the state when false is returned
	virtual bool print( ElementType type, const types::VariantConst& element)=0;

	/// \brief Get the current state
	/// \return the current state
	State state() const				{return m_state;}

	/// \brief Set output filter state with error message
	/// \param [in] s new state
	/// \param [in] msg (optional) error to set
	void setState( State s, const char* msg=0)	{m_state=s; setError(msg);}

private:
	State m_state;			//< state
};

/// \typedef TypedOutputFilterR
/// \brief Shared output filter (langbind::TypedOutputFilter) reference
typedef boost::shared_ptr<TypedOutputFilter> TypedOutputFilterR;


}}//namespace
#endif


