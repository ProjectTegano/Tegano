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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file serialize/directmapBase.hpp
///\brief Defines the non intrusive base class of serialization for the direct map

#ifndef _Wolframe_DIRECTMAP_BASE_HPP_INCLUDED
#define _Wolframe_DIRECTMAP_BASE_HPP_INCLUDED
#include "protocol/inputfilter.hpp"
#include "protocol/formatoutput.hpp"
#include <cstddef>
#include <string>
#include <map>

namespace _Wolframe {
namespace serialize {

class ProcessingContext
{
public:
	ProcessingContext() :m_endtagConsumed(false){}
	const char* getLastError() const {return m_lasterror;}
	const std::string content() const {return m_content;}
	void endTagConsumed( bool v)	{m_endtagConsumed=v;}
	bool endTagConsumed()		{return m_endtagConsumed;}
private:
	friend class DescriptionBase;
	char m_lasterror[ 256];
	std::string m_content;
	bool m_endtagConsumed;
};

class DescriptionBase
{
public:
	typedef void (*Parse)( const char* tag, void* obj, protocol::InputFilter& flt, ProcessingContext& ctx);
	typedef void (*Print)( const char* tag, const void* obj, protocol::FormatOutput*& out, std::string& buf);
	typedef bool (*IsAtomic)();

	DescriptionBase( const char* tn, std::size_t ofs, std::size_t sz, IsAtomic ia, Parse pa, Print pr)
		:m_typename(tn),m_ofs(ofs),m_size(sz),m_isAtomic(ia),m_parse(pa),m_print(pr){}
	DescriptionBase( const DescriptionBase& o)
		:m_typename(o.m_typename),m_ofs(o.m_ofs),m_size(o.m_size),m_elem(o.m_elem),m_isAtomic(o.m_isAtomic),m_parse(o.m_parse),m_print(o.m_print){}
	DescriptionBase()
		:m_typename(0),m_ofs(0),m_size(0),m_isAtomic(0),m_parse(0),m_print(0){}

	bool parse( const char* name, void* obj, protocol::InputFilter& in, ProcessingContext& ctx) const;
	bool print( const char* name, const void* obj, protocol::FormatOutput& out, ProcessingContext& ctx) const;

	bool isAtomic() const
	{
		return m_isAtomic();
	}
	std::size_t size() const
	{
		return m_size;
	}
public:
	const char* m_typename;
	std::size_t m_ofs;
	std::size_t m_size;
	typedef std::map<std::string,DescriptionBase> Map;
	Map m_elem;
	IsAtomic m_isAtomic;
	Parse m_parse;
	Print m_print;
};

}}//namespace
#endif
