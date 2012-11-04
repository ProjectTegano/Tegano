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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file databaseLanguage.cpp
///\brief Implementation of language definitions for embedded commands in transactions
#include "database/databaseLanguage.hpp"
#include "utils/miscUtils.hpp"
#include <string>
#include <iostream>
#include <sstream>

using namespace _Wolframe;
using namespace _Wolframe::db;

std::string LanguageDescription::stm_argument_reference( int index) const
{
	std::ostringstream rt;
	rt << "$" << index;
	return rt.str();
}

bool LanguageDescription::isEmbeddedStatement( std::string::const_iterator si, std::string::const_iterator se) const
{
	std::string::const_iterator start = si;
	while (si != se && *si > 0 && *si <= 32) ++si;
	std::string keyword = boost::algorithm::to_lower_copy( std::string( start, si));
	if (boost::iequals( keyword, "select")) return true;
	if (boost::iequals( keyword, "update")) return true;
	if (boost::iequals( keyword, "insert")) return true;
	if (boost::iequals( keyword, "delete")) return true;
	return false;
}

static const utils::CharTable g_optab( ";:-,.=)(<>[]{}/&%*|+-#?!$");

std::string LanguageDescription::parseEmbeddedStatement( std::string::const_iterator& si, std::string::const_iterator se) const
{
	std::string rt;
	const char* commentopr = eoln_commentopr();
	std::string tok;
	std::string::const_iterator start = si;
	char ch;
	while (0!=(ch=utils::parseNextToken( tok, si, se, g_optab)))
	{
		if (ch == ';') break;
		if (ch == commentopr[0])
		{
			std::string::const_iterator ti = si;
			std::size_t ci = 1;
			while (commentopr[ci] && ti != se && commentopr[ci] == *ti)
			{
				++ci;
				++ti;
			}
			if (!commentopr[ci])
			{
				// skip to end of line
				while (ti != se && *ti != '\n') ++ti;
				rt.append( std::string( start, si-1));
				start = si = ti;
				continue;
			}
		}
		break;

	}
	if (ch != ';') throw std::runtime_error( "embedded statement not terminated");
	rt.append( std::string( start, si-1));
	return rt;
}

