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
/// \file appdevel/doctypeModuleMacros.hpp
/// \brief Macros for a document type/format module definition

#include "module/baseObject.hpp"
#include "module/simpleObjectConstructor.hpp"
#include "cmdbind/doctypeDetector.hpp"

/// \brief Defines a Wolframe document format with a document type detector after the module includes section and module declaration header.
#define WF_DOCUMENT_FORMAT(NAME,CLASSDEF)\
{\
	class DetectorType\
		:public _Wolframe::cmdbind::DoctypeDetectorType\
		,public _Wolframe::module::BaseObjectInstance<_Wolframe::cmdbind::DoctypeDetectorType>\
	{\
	public:\
		DetectorType()\
			:_Wolframe::cmdbind::DoctypeDetectorType(NAME){}\
		virtual _Wolframe::cmdbind::DoctypeDetector* create() const\
		{\
			return new CLASSDEF();\
		}\
	};\
	class Constructor\
		:public _Wolframe::module::SimpleObjectConstructor\
	{\
	public:\
		Constructor( const std::string& className_)\
			: _Wolframe::module::SimpleObjectConstructor( _Wolframe::module::ObjectDescription::DOCTYPE_DETECTOR_OBJECT, className_){}\
		virtual _Wolframe::module::BaseObject* object() const\
		{\
			return new DetectorType();\
		}\
		static const _Wolframe::module::ObjectConstructor* impl()\
		{\
			static const Constructor rt( NAME "DocTypeDetection");\
			return &rt;\
		}\
	};\
	(*this)(&Constructor ::impl);\
}

