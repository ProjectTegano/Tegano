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
///\brief Implementation of templates describing printing of structures
///\file utils_printFormats.cpp
#include "utils/printFormats.hpp"

using namespace _Wolframe;
using namespace _Wolframe::utils;

const PrintFormat* utils::logPrintFormat()
{
	static const PrintFormat rt = {
		""  /*indent*/,
		" " /*newitem*/,
		"{" /*openstruct*/,
		"}" /*closestruct*/,
		":" /*endheader*/,
		"'" /*startvalue*/,
		"'" /*endvalue*/,
		"\\" /*escvalue*/,
		";" /*decldelimiter*/,
		" " /*itemdelimiter*/,
		"=" /*assign*/,
		60  /*maxitemsize*/
	};
	return &rt;
}

const PrintFormat* utils::ptreePrintFormat()
{
	static const PrintFormat rt = {
		"\t"  /*indent*/,
		"\n"  /*newitem*/,
		"{"   /*openstruct*/,
		"}"   /*closestruct*/,
		":"   /*endheader*/,
		"'"   /*startvalue*/,
		"'"   /*endvalue*/,
		"\\"   /*escvalue*/,
		";"   /*decldelimiter*/,
		" "   /*itemdelimiter*/,
		"="   /*assign*/,
		0     /*maxitemsize*/
	};
	return &rt;
}

std::string utils::getLogString( const types::Variant& val, std::size_t maxsize)
{
	std::string rt;
	rt.append( val.tostring());
	if (rt.size() > maxsize)
	{
		rt.resize( maxsize);
		rt.append( "...");
	}
	return rt;
}


