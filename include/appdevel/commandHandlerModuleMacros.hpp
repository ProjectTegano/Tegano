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
/// \file appdevel/commandHandlerModuleMacros.hpp
/// \brief Macros for a module for a configurable command handler
#include "module/moduleInterface.hpp"
#include "module/configuredObjectConstructor.hpp"
#include "config/configurationObject.hpp"
#include "cmdbind/commandHandler.hpp"
#include "processor/procProviderInterface.hpp"
#include "logger/logger-v1.hpp"

/// \brief Defines a Wolframe command handler object after the includes section.
#define WF_COMMAND_HANDLER(NAME,CONFIG_SECTION,CONFIG_KEYWORD,CLASSDEF,CONFIGDEF)\
{\
	class ModuleObjectEnvelope \
		:public CLASSDEF \
		,public _Wolframe::module::BaseObject\
	{\
	public:\
		ModuleObjectEnvelope( const CONFIGDEF* cfg) :CLASSDEF(cfg){}\
	};\
	class Constructor\
		:public _Wolframe::module::ConfiguredObjectConstructor\
	{\
	public:\
		Constructor( const std::string& className_, const std::string& configSection_, const std::string& configKeyword_)\
			:_Wolframe::module::ConfiguredObjectConstructor( _Wolframe::module::ObjectDescription::CMD_HANDLER_OBJECT, className_, configSection_, configKeyword_){}\
		virtual _Wolframe::module::BaseObject* object( const _Wolframe::config::ConfigurationObject& cfgi) const\
		{\
			const CONFIGDEF* cfg = dynamic_cast<const CONFIGDEF*>(&cfgi);\
			if (!cfg)\
			{\
				LOG_ERROR << "internal: wrong configuration interface passed to " NAME " command handler constructor";\
				return 0;\
			}\
			return new ModuleObjectEnvelope( cfg);\
		}\
		virtual _Wolframe::config::ConfigurationObject* configuration() const\
		{\
			return new CONFIGDEF( NAME "CommandHandler", CONFIG_SECTION, CONFIG_KEYWORD);\
		}\
		static const _Wolframe::module::ObjectConstructor* impl()\
		{\
			static const Constructor rt( NAME "CommandHandler", CONFIG_SECTION, CONFIG_KEYWORD);\
			return &rt;\
		}\
	};\
	(*this)(&Constructor ::impl);\
}


