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
//\file Wolframe_/syncObjectList.hpp
//\brief Interface to shared and synchronized list of objects
#ifndef _Wolframe_SYNC_OBJECT_LIST_HPP_INCLUDED
#define _Wolframe_SYNC_OBJECT_LIST_HPP_INCLUDED
#include <list>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

namespace _Wolframe {
namespace types {

//\class SyncObjectList
//\brief Shared and synchronized list of objects
template <class OBJ>
class SyncObjectList
{
public:
	typedef typename std::list<OBJ>::iterator Reference;
	
	//\brief Constructor
	SyncObjectList()
	{}

	//\brief Insert object into the list
	//\return a handle to the object
	Reference insert( OBJ obj)
	{
		boost::mutex::scoped_lock lock( m_mutex);
		m_list.push_front( obj);
		return m_list.begin();
	}

	void release( const Reference& objref)
	{
		boost::mutex::scoped_lock lock( m_mutex);
		m_list.erase( objref);
	}

private:
	boost::mutex m_mutex;		//< mutex for mutual exclusion of writes
	std::list<OBJ> m_list;		//< list of object references
};

}}//namespace
#endif


