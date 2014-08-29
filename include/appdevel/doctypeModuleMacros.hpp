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
#include "module/moduleInterface.hpp"
#include "appdevel/module/doctypeDetectorBuilder.hpp"

/// \brief Defines a Wolframe document format with a document type detector after the module includes section and module declaration header.
#define WF_DOCUMENT_FORMAT( DOCFORMATNAME, DETECTORCLASS)\
{\
	struct Constructor\
	{\
		static _Wolframe::cmdbind::DoctypeDetector* create()\
		{\
			return new DETECTORCLASS();\
		}\
		static const _Wolframe::module::BuilderBase* impl()\
		{\
			static const _Wolframe::module::DoctypeDetectorBuilder rt( DOCFORMATNAME "Detector", DOCFORMATNAME, create);\
			return &rt;\
		}\
	};\
	(*this)(&Constructor ::impl);\
}

