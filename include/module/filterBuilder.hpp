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
///\file module/filterBuilder.hpp
///\brief Interface template for object builder of filters
#ifndef _Wolframe_MODULE_FILTER_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#define _Wolframe_MODULE_FILTER_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDEDd
#include "filter/filter.hpp"
#include "moduleInterface.hpp"
#include "constructor.hpp"

namespace _Wolframe {
namespace module {

class FilterConstructor :public SimpleObjectConstructor< langbind::Filter >
{
public:
	FilterConstructor( const char* name_, langbind::CreateFilterPtrFunc filterFunc_ )
		: m_name(name_)
		, m_function(filterFunc_) {}

	virtual ~FilterConstructor(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return FILTER_OBJECT;
	}
	virtual const char* identifier() const
	{
		return m_name.c_str();
	}
	virtual langbind::Filter* object( const std::string& arg) const
	{
		return m_function( m_name, arg );
	}

private:
	const std::string m_name;
	const std::string m_arg;
	langbind::CreateFilterPtrFunc m_function;
};

class FilterBuilder :public SimpleBuilder
{
public:
	FilterBuilder( const char* name_, langbind::CreateFilterPtrFunc createFunc_)
		:SimpleBuilder( name_)
		,m_createFunc(createFunc_){}

	virtual ~FilterBuilder(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return ObjectConstructorBase::FILTER_OBJECT;
	}
	virtual ObjectConstructorBase* constructor()
	{
		return new FilterConstructor( m_builderName, m_createFunc);
	}
private:
	langbind::CreateFilterPtrFunc m_createFunc;
};

}}//namespace

#endif
