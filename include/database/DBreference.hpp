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
// Reference to a database
//

#ifndef _DB_REFERENCE_HPP_INCLUDED
#define _DB_REFERENCE_HPP_INCLUDED

#include "database/database.hpp"

namespace _Wolframe {
namespace db {

/// database reference class
/// nothe that this is a configuration class only
struct ReferenceConfig : public DatabaseConfig
{
	std::string	m_ref;
public:
	DatabaseType type() const			{ return DBTYPE_REFERENCE; }

	ReferenceConfig( const char* name, const char* logParent, const char* logName )
		: DatabaseConfig( name, logParent, logName )	{}
	~ReferenceConfig()					{}

	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
};

}} // namespace _Wolframe::db

#endif // _DB_REFERENCE_HPP_INCLUDED
