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
/// \file appdevel/authorizationModuleMacros.hpp
/// \brief Macros for a module for defining an authorization methods
#include "module/moduleInterface.hpp"
#include "module/configuredObjectConstructor.hpp"
#include "config/configurationObject.hpp"
#include <boost/lexical_cast.hpp>
#include "aaaa/authorizationUnit.hpp"
#include "logger/logger-v1.hpp"

/// \brief Defines an authorizer
#define WF_AUTHORIZER(NAME,UNITCLASS,CONFIGCLASS)\
{\
	class ModuleObjectEnvelope \
		:public UNITCLASS \
		,public _Wolframe::module::BaseObject\
	{\
	public:\
		ModuleObjectEnvelope( const CONFIGCLASS* cfg) :UNITCLASS(cfg){}\
	};\
	class Constructor\
		:public _Wolframe::module::ConfiguredObjectConstructor\
	{\
	public:\
		Constructor()\
			:_Wolframe::module::ConfiguredObjectConstructor( _Wolframe::module::ObjectDescription::AUTHORIZATION_OBJECT, NAME "Authorizer", "authorization", NAME){}\
		virtual _Wolframe::module::BaseObject* object( const _Wolframe::config::ConfigurationObject& cfgi) const\
		{\
			const CONFIGCLASS* cfg = dynamic_cast<const CONFIGCLASS*>(&cfgi);\
			if (!cfg)\
			{\
				LOG_ERROR << "internal: wrong configuration interface passed to " NAME " AuthorizationUnit constructor";\
				return 0;\
			}\
			return new ModuleObjectEnvelope( cfg);\
		}\
		virtual _Wolframe::config::ConfigurationObject* configuration() const\
		{\
			return new CONFIGCLASS( NAME "Authorization", "Authorization", NAME);\
		}\
		static const _Wolframe::module::ObjectConstructor* impl()\
		{\
			static const Constructor rt;\
			return &rt;\
		}\
	};\
	(*this)(&Constructor ::impl);\
}



