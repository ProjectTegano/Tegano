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
/*
 *  Password-Based Key Derivation Function 2 (PKCS #5 v2.0).
 *  See RFC 2898 (http://tools.ietf.org/html/rfc2898)
 */

#ifndef _PBKDF2_H_INCLUDED
#define _PBKDF2_H_INCLUDED

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int pbkdf2( const unsigned char *salt, size_t saltLen,
	    const unsigned char *password, size_t pwdLen,
	    unsigned char *derivedKey, size_t dkLen, unsigned int rounds );

#ifdef __cplusplus
}
#endif

#endif // _PBKDF2_H_INCLUDED