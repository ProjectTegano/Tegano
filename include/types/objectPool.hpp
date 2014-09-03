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
/// \brief Thread capable pool of objects
/// \file types/objectPool.hpp

#ifndef _Wolframe_TYPES_OBJECT_POOL_HPP_INCLUDED
#define _Wolframe_TYPES_OBJECT_POOL_HPP_INCLUDED
#include <vector>
#include <string>
#include <stdexcept>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace types {

/// \brief Timeout exception for object pools
class ObjectPoolTimeoutException
	:public std::runtime_error
{
public:
	ObjectPoolTimeoutException()
		:std::runtime_error( "object pool timeout exception"){}
};

/// \class ObjectPoolBase
/// \brief Base class implementing a pool of objects
class ObjectPoolBase
{
public:
	class ObjectRef
		:public boost::shared_ptr<void>
	{
	public:
		ObjectRef(){}
		ObjectRef( ObjectPoolBase* pool, void* ptr);
		ObjectRef( const ObjectRef& o)
			:boost::shared_ptr<void>( o){}

	private:
		static void readdObjectToPool( ObjectPoolBase* pool, void* obj);
	};

public:
	typedef void (*DeleteObjectTypeFunc)( void*);

	explicit ObjectPoolBase( DeleteObjectTypeFunc deleteObject_, unsigned timeout_);

	~ObjectPoolBase();

	std::size_t size() const;

	ObjectRef get();

	void push( void* obj);

	void setTimeout( unsigned int timeout_)
	{
		m_timeout = timeout_;
	}

	void clear();

private:
	DeleteObjectTypeFunc m_deleteObject;	///< Object destructor
	std::vector<void*> m_availList;		///< List of available objects
	mutable boost::mutex m_mutex;		///< Condition variable associated mutex
	boost::condition_variable m_cond;	///< Condition variable
	unsigned int m_timeout;			///< Acquire timeout
};


/// \class ObjectPool
/// \tparam ObjectType type of object stored in the pool
/// \brief Class represening a pool of objects
template <typename ObjectType>
class ObjectPool
	:private ObjectPoolBase
{
public:
	class ObjectRef
		:public ObjectPoolBase::ObjectRef
	{
	public:
		ObjectRef(){}
		ObjectRef( const ObjectPoolBase::ObjectRef& o)
			:ObjectPoolBase::ObjectRef( o){}

		const ObjectType* get() const		{return (const ObjectType*)ObjectPoolBase::ObjectRef::get();}
		ObjectType* get()			{return (ObjectType*)ObjectPoolBase::ObjectRef::get();}
		const ObjectType* operator->() const	{return get();}
		ObjectType* operator->()		{return get();}
	};

	typedef void (*DeleteObjectTypeFunc)( void*);

public:
	explicit ObjectPool( unsigned timeout_=0)
		:ObjectPoolBase( &std_deleteObjectType, timeout_){}

	explicit ObjectPool( DeleteObjectTypeFunc deleteObject, unsigned int timeout_)
		:ObjectPoolBase( deleteObject, timeout_){}
	
	std::size_t size() const		{return ObjectPoolBase::size();}
	ObjectRef get()				{return ObjectRef( ObjectPoolBase::get());}
	void push( ObjectType* obj)		{ObjectPoolBase::push( (void*)obj);}
	void setTimeout( unsigned int timeout_)	{ObjectPoolBase::setTimeout( timeout_);}
	void clear()				{ObjectPoolBase::clear();}

private:
	static void std_deleteObjectType( void* obj)
	{
		delete reinterpret_cast<ObjectType*>(obj);
	}
};

}} // namespace
#endif

