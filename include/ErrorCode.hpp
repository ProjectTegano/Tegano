/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
//
// Error codes
//

#ifndef _ERROR_CODES_HPP_INCLUDED
#define _ERROR_CODES_HPP_INCLUDED

namespace _Wolframe	{

class ErrorCode	{
public:
	enum	{
		OK = 0,
		FAILURE = 1
	};
};


class ErrorLevel	{
public:
	enum	{
		NORMAL,		/// the system is working normally
		WARNING,	/// ok, a warning is a warning
		/* Aba: gets expanded on Windows, ERROR is a macro! */
		_ERROR,		/// an error has occured, the system might be able to continue
		SEVERE,		/// current processing is doomed. system not yet
		CRITICAL,	/// the system is doomed now
		FATAL		/// system cannot continue
	};
};

} // namespace _Wolframe

#endif // _ERROR_CODES_HPP_INCLUDED
