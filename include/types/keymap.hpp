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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file types/keymap.hpp
///\brief template for map with case insensitive key strings

#ifndef _Wolframe_TYPES_KEYMAP_HPP_INCLUDED
#define _Wolframe_TYPES_KEYMAP_HPP_INCLUDED
#include "types/traits.hpp"
#include <map>
#include <string>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>
#include <boost/detail/select_type.hpp>

namespace _Wolframe {
namespace types {

struct keystring
	:public std::string
{
	keystring( const std::string& o)
		:std::string( boost::to_lower_copy( o)){}
	keystring( const keystring& o)
		:std::string( o){}
	keystring(){}
};

template <typename ValueType>
struct keymap
	:public std::map<keystring,ValueType>
{
	typedef std::map<keystring,ValueType> Parent;

	keymap(){}
	keymap( const keymap& o)
		:std::map<keystring,ValueType>(o){}
	keymap( const std::map<std::string,ValueType>& o)
	{
		std::copy( o.begin(), o.end(), std::inserter( *this, Parent::end()));
	}

	void insert( const keystring& key, const ValueType& value)
	{
		if (Parent::find( key) != Parent::end()) throw std::runtime_error( std::string( "duplicate definition of '") + key + "'");
		Parent::operator[](key) = value;
	}

	template <class KeyList>
	typename boost::enable_if_c<
		boost::is_convertible<typename KeyList::const_iterator::value_type, keystring>::value
		&& types::traits::has_pre_increment<typename KeyList::const_iterator>::value
		&& boost::has_dereference<typename KeyList::const_iterator>::value
		,void>::type
	insert( const KeyList& keylist, const ValueType& value)
	{
		typename KeyList::const_iterator ki = keylist.begin(), ke =  keylist.end();
		for (; ki != ke; ++ki) insert( *ki, value);
	}

	template <class KeyList>
	typename boost::enable_if_c<
		types::traits::is_back_insertion_sequence<KeyList>::value
		,void>::type
	getkeys( KeyList& rt)
	{
		typename Parent::const_iterator ki = Parent::begin(), ke = Parent::end();
		for (; ki != ke; ++ki) rt.push_back( ki->first);
	}

	template <class KeyList>
	typename boost::enable_if_c<
		types::traits::is_back_insertion_sequence<KeyList>::value
		,KeyList>::type
	getkeys()
	{
		KeyList rt;
		getkeys(rt);
		return rt;
	}
};

}} //namespace
#endif

