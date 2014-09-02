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
/// \file databaseProviderImpl.hpp

#ifndef _DATABASE_PROVIDER_IMPLEMENTATION_HPP_INCLUDED
#define _DATABASE_PROVIDER_IMPLEMENTATION_HPP_INCLUDED

#include "config/configurationBase.hpp"
#include "database/databaseProvider.hpp"
#include <vector>

namespace _Wolframe {
namespace db {

/// \brief Database provider implementation
class DatabaseProviderImpl
	:public DatabaseProvider
	,private boost::noncopyable
{
public:
	DatabaseProviderImpl( const std::vector<config::NamedConfiguration*>& config,
				const module::ModuleDirectory* modules );
	virtual ~DatabaseProviderImpl();
	virtual Database* database( const std::string& ID ) const;

private:
	std::vector< DatabaseUnit* > m_db;
};

}} // namespace _Wolframe::db
#endif


