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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
/// \file module/configuredObjectConstructorTemplate.hpp
/// \brief Template to define a configured object constructor by type

#include "module/configuredObjectConstructor.hpp"
#include "config/configurationObject.hpp"
#include <boost/shared_ptr.hpp>
#include <stdexcept>

#ifndef _CONFIGURED_OBJECT_CONSTRUCTOR_TEMPLATE_HPP_INCLUDED
#define _CONFIGURED_OBJECT_CONSTRUCTOR_TEMPLATE_HPP_INCLUDED

namespace _Wolframe {
namespace module {

#error DEPRECATED 

/// \brief Template to define a constructor of a configured object by type
/// \remark OBJECT has to be derived from module::BaseObject
/// \remark OBJECT has to constructable as OBJECT( const CONFIG*)
/// \remark CONFIG has to be derived from config::ConfigurationObject
/// \todo Concept checks to verify conditions on OBJECT and CONFIG
template <ObjectDescription::TypeId TYPEID, class OBJECT, class CONFIG>
struct ConfiguredObjectConstructorTemplate
	:public ConfiguredObjectConstructor
{
	ConfiguredObjectConstructorTemplate( const std::string& className_, const std::string& configSection_, const std::string& configKeyword_)
		:ConfiguredObjectConstructor( TYPEID, className_, configSection_, configKeyword_){}

	virtual BaseObject* object( const config::ConfigurationObject& cfg_) const
	{
		const CONFIG* cfg = dynamic_cast<const CONFIG*>(&cfg_);
		if (!cfg) throw std::logic_error( std::string("RTTI cast erron in configured object constructor of '") + className() + "'");
		return new OBJECT( cfg);
	}

	virtual config::ConfigurationObject* configuration() const 
	{
		const module::ObjectDescription* description = this;
		return new CONFIG( description->className(), description->configSection(), description->configKeyword());
	}

	static const ObjectConstructor* create()
	{
		static const ConfiguredObjectConstructorTemplate<TYPEID,OBJECT,CONFIG> rt;
		return &rt;
	}
};

}}// namespace
#endif
