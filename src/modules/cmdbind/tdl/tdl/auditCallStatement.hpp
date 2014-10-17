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
///\brief Definition of embedded database statement parsing result
///\file tdl/auditCallStatement.hpp
#ifndef _DATABASE_TDL_AUDIT_CALL_STATEMENT_HPP_INCLUDED
#define _DATABASE_TDL_AUDIT_CALL_STATEMENT_HPP_INCLUDED
#include "database/databaseLanguage.hpp"
#include "tdl/auditElementReference.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace db {
namespace tdl {

struct AuditCallStatement
{
	std::string name;				//< function name
	std::vector<AuditElementReference> params;	//< list of call arguments

	AuditCallStatement(){}
	AuditCallStatement( const AuditCallStatement& o)
		:name(o.name),params(o.params){}
	AuditCallStatement( const std::string& name_, const std::vector<AuditElementReference>& params_)
		:name(name_),params(params_){}

	void clear();

	static AuditCallStatement parse( std::string::const_iterator& ci, std::string::const_iterator ce);
};

}}}//namespace
#endif

