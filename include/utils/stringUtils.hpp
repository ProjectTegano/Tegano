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
/// \file utils/stringUtils.hpp
/// \brief Utility functions for string handling (Mainly to hide intrusive stuff. Implementation is done with help of STL/boost)

#ifndef _WOLFRAME_STRING_UTILS_HPP_INCLUDED
#define _WOLFRAME_STRING_UTILS_HPP_INCLUDED
#include <string>
#include <vector>
#include <iostream>

namespace _Wolframe {
namespace langbind {
class TypedInputFilter;
}
namespace utils {
class PrintFormat;

/// \brief Split the string, ingoring empty parts of the result (susequent split characters treated as one)
/// \param[out] res result of the split
/// \param[in] inp string to split
/// \param[in] splitchr set of characters to split with (each of them is one separating character)
void splitString( std::vector<std::string>& res, const std::string& inp, const char* splitchr);

/// \brief Split the string, ingoring empty parts of the result (susequent split characters treated as one)
/// \param[out] res result of the split
/// \param[in] begin begin input iterator
/// \param[in] end end input iterator
/// \param[in] splitchr set of characters to split with (each of them is one separating character)
void splitString( std::vector<std::string>& res, std::string::const_iterator begin, std::string::const_iterator end, const char* splitchr);

/// \brief Iterate on a filter and return its contents as string
/// \remark Interface does not allow chunkwise processing. Content of input filter has to be complete
/// \param[in,out] flt filter to iterate on to print the contents
/// \param[in] format format to print with
/// \return the printed content of filter 'flt' as string
std::string filterToString( langbind::TypedInputFilter& flt, const PrintFormat* format);

/// \brief Iterate on a filter and print its contents to an STL stream
/// \param[out] dest stream to print to
/// \param[in,out] flt filter to iterate on to print the contents
/// \param[in] format format to print with
/// \return true on success, false if error or input of filter is not complete (inspect state of 'flt')
bool printFilterToStream( std::ostream& dest, langbind::TypedInputFilter& flt, const PrintFormat* format);

}} //namespace
#endif
