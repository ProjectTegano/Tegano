/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
//\file connectionType.hpp
//\brief Connection type interface
#ifndef _Wolframe_CONNECTION_TYPE_HPP_INCLUDED
#define _Wolframe_CONNECTION_TYPE_HPP_INCLUDED
#include <list>
#include "types/syncObjectList.hpp"

namespace _Wolframe {
namespace net {

class ConnectionType;
typedef types::BoundedSyncObjectList<ConnectionType*> ConnectionTypeList;

class ConnectionType
{
public:

	explicit ConnectionType( ConnectionTypeList* listref_)
		:m_listref(listref_)
		,m_inserted(false){}

	virtual ~ConnectionType()
	{
		if (m_inserted) m_listref->release( m_elemref);
	}

	bool registerConnection()
	{
		if (m_inserted) throw std::logic_error("connection registered twice");
		return m_inserted=m_listref->insert( this, m_elemref);
	}

private:
	ConnectionTypeList::ElementReference m_elemref;
	ConnectionTypeList* m_listref;
	bool m_inserted;
};

}}
#endif

