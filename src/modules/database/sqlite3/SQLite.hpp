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
//
// Wolframe SQLite client
//

#ifndef _SQLITE_HPP_INCLUDED
#define _SQLITE_HPP_INCLUDED

#include "SQLiteConfig.hpp"
#include "database/database.hpp"
#include "database/transaction.hpp"
#include <vector>
#include "types/objectPool.hpp"
#include "sqlite3.h"

#ifdef _WIN32
#pragma warning(disable:4250)
#endif

namespace _Wolframe {
namespace db {

struct SQLiteLanguageDescription :public LanguageDescriptionSQL
{
	///\brief String used for declaring a reference to an argument by index (starting with 1).
	virtual std::string stm_argument_reference( int index) const
	{
		std::ostringstream rt;
		rt << "$" << index;
		return rt.str();
	}
};

class SQLiteDatabase
	:public Database
{
public:
	SQLiteDatabase( const std::string& id_, const std::string& filename_,
			bool foreignKeys_, bool profiling_,
			unsigned short connections_,
			const std::vector<std::string>& extensionFiles_ );
	SQLiteDatabase( const SQLiteConfig* config);
	 ~SQLiteDatabase();

	Transaction* transaction( const std::string& name_);

	virtual const LanguageDescription* getLanguageDescription() const
	{
		static SQLiteLanguageDescription langdescr;
		return &langdescr;
	}

	typedef types::ObjectPool<sqlite3>::ObjectRef Connection;
	Connection newConnection()
	{
		return m_connPool.get();
	}

private:
	void init( const SQLiteConfig& config);

private:
	const std::string	m_id;
	const std::string	m_filename;
	types::ObjectPool<sqlite3> m_connPool;		///< pool of connections
	std::vector<std::string> m_extensionFiles;	///< Sqlite extensions
};

}} // _Wolframe::db

#endif // _SQLITE_HPP_INCLUDED
