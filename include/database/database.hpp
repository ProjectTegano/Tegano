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
/// \file database.hpp
/// \brief Wolframe base database class

#ifndef _DATABASE_HPP_INCLUDED
#define _DATABASE_HPP_INCLUDED
#include "database/transaction.hpp"
#include "database/databaseLanguage.hpp"
#include <string>
#include <iostream>
#include <sstream>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace db {

/// \class Database
/// \brief Base class for database interface. All databases should provide this interface
class Database
{
public:
	Database( const::std::string& name_, const::std::string& id_)
		:m_name(name_),m_id(id_){}

	virtual ~Database(){}

	/// \brief Get a database transaction object
	virtual Transaction* transaction( const std::string& name ) const= 0;

	virtual const LanguageDescription* getLanguageDescription() const=0;

	const std::string& name() const	{return m_name;}
	const std::string& id() const	{return m_id;}

private:
	std::string m_name;
	std::string m_id;
};

typedef boost::shared_ptr<Database> DatabaseR;


/// \class DatabaseUnit
/// \brief This is the base class for virtual constructors of databases
class DatabaseUnit
{
public:
	virtual ~DatabaseUnit(){}

	/// \brief The database identifier. This is the configured name.
	virtual const std::string& id() const = 0;

	/// \brief The actual database object.
	virtual const Database* database() = 0;
};

}} // namespace _Wolframe::db

#endif // _DATABASE_HPP_INCLUDED
