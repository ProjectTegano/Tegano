/************************************************************************
Copyright (C) 2011 - 2013 Project Wolframe.
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
///\file ddl/structType.hpp
///\brief Defines a non intrusive structure type for values in the DDLs used for forms

#ifndef _Wolframe_DDL_STRUCTTYPE_HPP_INCLUDED
#define _Wolframe_DDL_STRUCTTYPE_HPP_INCLUDED
#include <string>
#include <utility>
#include <vector>
#include <stdexcept>
#include <cstddef>
#include <boost/lexical_cast.hpp>
#include "types/countedReference.hpp"
#include "types/variantStruct.hpp"
#include "types/variantStructDescription.hpp"

namespace _Wolframe {
namespace ddl {

class FormDescription
	:public types::VariantStructDescription
{
public:
	///\brief Default constructor
	FormDescription(){}
	///\brief Destructor
	virtual ~FormDescription(){}
	///\brief Constructor
	FormDescription( const std::string& ddlname_, const std::string& name_)
		:m_name(name_)
		,m_ddlname(ddlname_){}
	///\brief Copy constructor
	FormDescription( const FormDescription& o)
		:types::VariantStructDescription(o)
		,m_name(o.m_name)
		,m_ddlname(o.m_ddlname){}

	///\brief Assignement operator
	///\param[in] o object to copy
	FormDescription& operator= ( const FormDescription& o)
	{
		types::VariantStructDescription::operator=( o);
		m_name = o.m_name;
		m_ddlname = o.m_ddlname;
		return *this;
	}

	const char* xmlRoot() const
	{
		return (size() == 1 && !m_name.empty())?begin()->name:0;
	}

	const std::string& name() const
	{
		return m_name;
	}

	const std::string& ddlname() const
	{
		return m_ddlname;
	}

private:
	std::string m_name;
	std::string m_ddlname;
};

class Form
	:public types::VariantStruct
{
public:
	///\brief Default constructor
	Form()
		:m_description(0){}
	///\brief Destructor
	virtual ~Form(){}
	///\brief Constructor
	explicit Form( const FormDescription* description_)
		:m_description(description_){}
	///\brief Copy constructor
	Form( const Form& o)
		:types::VariantStruct(o)
		,m_description(o.m_description){}

	///\brief Assignement operator
	///\param[in] o object to copy
	Form& operator= ( const Form& o)
	{
		types::VariantStruct::operator=( o);
		m_description = o.m_description;
		return *this;
	}

	const FormDescription* description() const
	{
		return m_description;
	}

	void print( std::ostream& out, size_t level=0) const;

private:
	const FormDescription* m_description;
};

typedef types::CountedReference<Form> FormR;

}}//namespace
#endif

