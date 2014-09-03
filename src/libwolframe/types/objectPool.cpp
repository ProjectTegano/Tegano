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
#include "types/objectPool.hpp"
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/bind.hpp>
/*[-]*/#include <iostream>

using namespace _Wolframe;
using namespace _Wolframe::types;

ObjectPoolBase::ObjectPoolBase( DeleteObjectTypeFunc deleteObject_, unsigned timeout_)
	:m_deleteObject(deleteObject_)
	,m_timeout(timeout_){}

ObjectPoolBase::~ObjectPoolBase()
{
	boost::unique_lock<boost::mutex> lock( m_mutex );
	while (m_availList.size())
	{
		if (m_deleteObject) m_deleteObject( m_availList.back());
		m_availList.pop_back();
	}
}

std::size_t ObjectPoolBase::size() const
{
	return m_availList.size();
}

ObjectPoolBase::ObjectRef ObjectPoolBase::get()
{
	for(;;)
	{
		boost::unique_lock<boost::mutex> lock( m_mutex );
		if ( !m_availList.empty())	{
			void* obj = m_availList.back();
			m_availList.pop_back();
			ObjectRef rt( this, obj);
			lock.unlock();
			return rt;
		}
		else	{
			if ( m_timeout == 0 )	{
				while( m_availList.empty() )
					m_cond.wait( lock );
			}
			else {
				boost::system_time absTime = boost::get_system_time()
						+ boost::posix_time::seconds( m_timeout );
				while( m_availList.empty() )
					if ( ! m_cond.timed_wait( lock, absTime ))
						throw ObjectPoolTimeoutException();
			}
		}
	}
}

void ObjectPoolBase::push( void* obj)
{
	boost::lock_guard<boost::mutex> lock( m_mutex );
	m_availList.push_back( obj);
	m_cond.notify_one();
}

void ObjectPoolBase::ObjectRef::readdObjectToPool( ObjectPoolBase* pool, void* obj)
{
	pool->push( obj);
}

ObjectPoolBase::ObjectRef::ObjectRef( ObjectPoolBase* pool, void* ptr)
	:boost::shared_ptr<void>( ptr, boost::bind( readdObjectToPool, pool, _1))
{}

void ObjectPoolBase::clear()
{
	boost::lock_guard<boost::mutex> lock( m_mutex );
	m_availList.clear();
}

