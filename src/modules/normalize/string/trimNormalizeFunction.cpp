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
///\file trimNormalizeFunction.cpp
#include "trimNormalizeFunction.hpp"
#include "types/variant.hpp"

using namespace _Wolframe;
using namespace langbind;

TrimNormalizeFunction::TrimNormalizeFunction( const std::vector<types::Variant>& arg)
{
	if (!arg.empty()) std::runtime_error( "no arguments expected for string normalizer 'trim'");
}

types::Variant TrimNormalizeFunction::execute( const types::Variant& inp) const
{
	if (inp.type() == types::Variant::String)
	{
		std::string str( inp.tostring());
		std::string::const_iterator ii = str.begin(), ee = str.end();
		while (ii != ee && (unsigned char)*ii <= 32) ++ii;
		std::string::const_iterator ti = ii, te = ii;
		for (; ii != ee; ++ii)
		{
			if ((unsigned char)*ii > 32) te = ii+1;
		}
		if (ti == str.begin() && te == str.end())
		{
			return str;
		}
		else
		{
			return std::string( ti, te);
		}
	}
	else
	{
		return inp;
	}
}

