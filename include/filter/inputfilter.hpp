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
/// \file filter/inputfilter.hpp
/// \brief Interface for input filter

#ifndef _Wolframe_FILTER_INPUTFILTER_INTERFACE_HPP_INCLUDED
#define _Wolframe_FILTER_INPUTFILTER_INTERFACE_HPP_INCLUDED
#include "types/sharedReference.hpp"
#include "types/docmetadata.hpp"
#include "filter/filterbase.hpp"
#include <string>
#include <stdexcept>
#include <cstring>
#include <cstdlib>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

/// \class InputFilter
/// \brief Input filter
class InputFilter
	:public FilterBase
{
public:
	/// \enum State
	/// \brief State of the input filter
	enum State
	{
		Start,		///< state after first initialization
		Open,		///< serving data - normal input processing
		EndOfMessage,	///< have to yield processing because end of message reached
		Error		///< have to stop processing with an error
	};

	/// \brief Constructor
	explicit InputFilter( const char* name_)
		:FilterBase(name_)
		,m_state(Start)
		,m_metadata( new types::DocMetaData()){}

	/// \brief Constructor
	InputFilter( const char* name_, const types::DocMetaData& metadata)
		:FilterBase(name_)
		,m_state(Start)
		,m_metadata( new types::DocMetaData( metadata)){}

	/// \brief Copy constructor
	/// \param[in] o input filter to copy
	InputFilter( const InputFilter& o)
		:FilterBase(o)
		,m_state(o.m_state)
		,m_metadata( new types::DocMetaData( *o.m_metadata)){}

	/// \brief Destructor
	virtual ~InputFilter(){}

	/// \brief Get a self copy
	/// \return allocated pointer to copy of this
	virtual InputFilter* copy() const=0;

	/// \brief Declare the next input chunk to the filter
	/// \param [in] ptr the start of the input chunk
	/// \param [in] size the size of the input chunk in bytes
	/// \param [in] end true, if end of input (the last chunk) has been reached.
	virtual void putInput( const void* ptr, std::size_t size, bool end)=0;

	/// \brief Get next element
	/// \param [out] type element type parsed
	/// \param [out] element pointer to element returned
	/// \param [out] elementsize size of the element returned
	/// \return true, if success, false, if not.
	/// \remark Check the filter state when false is returned
	virtual bool getNext( ElementType& type, const void*& element, std::size_t& elementsize)=0;

	/// \brief Get the current state
	/// \return the current state
	State state() const						{return m_state;}

	/// \brief Set input filter state with error message
	/// \param [in] s new state
	/// \param [in] msg (optional) error to set
	void setState( State s, const char* msg=0)			{m_state=s; setError(msg);}

	/// \brief Get the document meta data if available
	/// \return the metadata reference if already available, null, if not (if we have to feed more input or there was an error).
	/// \remark Check the filter state for error when null is returned
	virtual const types::DocMetaData* getMetaData()=0;

	/// \brief Get a shared reference to the document meta data to link output filter to input filer meta data
	virtual const types::DocMetaDataR& getMetaDataRef() const	{return m_metadata;}

	/// \brief Set a document meta data attribute
	void setAttribute( const std::string& name_, const std::string& value_)
	{
		m_metadata->setAttribute( name_, value_);
	}

	/// \brief Set a document type
	void setDoctype( const std::string& doctype_)
	{
		m_metadata->setDoctype( doctype_);
	}

	/// \brief Validate the meta data passed to be correct for this filter
	/// \return true, if the meta data defined in the argument structure are correct for this filter
	virtual bool checkMetaData( const types::DocMetaData&)
	{
		return true;
	}

private:
	State m_state;				///< state
	types::DocMetaDataR m_metadata;		///< meta data reference
};

/// \typedef InputFilterR
/// \brief Shared input filter (langbind::InputFilter) reference 
typedef types::SharedReference<InputFilter> InputFilterR;


}}//namespace
#endif


