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
///\brief Interface for normalize functions
///\file langbind/normalizeFunction.hpp
#ifndef _LANGBIND_NORMALIZE_FUNCTION_HPP_INCLUDED
#define _LANGBIND_NORMALIZE_FUNCTION_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "types/countedReference.hpp"

namespace _Wolframe {
namespace langbind {

struct NormalizeFunction
{
	NormalizeFunction(){}
	virtual ~NormalizeFunction(){}

	virtual std::string execute( const std::string& i) const=0;
};

typedef types::CountedReference<NormalizeFunction> NormalizeFunctionR;
class ResourceHandle;

///\param[in,out] rshnd normalization resources handle
///\param[in] description transaction description source
typedef NormalizeFunction* (*CreateNormalizeFunction)( ResourceHandle& reshnd, const std::string& description);

}}
#endif


