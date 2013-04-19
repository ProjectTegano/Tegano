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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file types/variantStruct.hpp
///\brief Structures of variant types

#ifndef _Wolframe_TYPES_VARIANT_STRUCT_HPP_INCLUDED
#define _Wolframe_TYPES_VARIANT_STRUCT_HPP_INCLUDED
#include <string>
#include "types/variant.hpp"

namespace _Wolframe {
namespace types {

///\brief Forward declaration for Variant
class StructDescription;

///\brief Structure of Variant type
class VariantStruct :public Variant
{
public:
	///\enum Type
	///\brief Typed filter element type
	enum Type
	{
		bool_=Variant::bool_,
		double_=Variant::double_,
		int_=Variant::int_,
		uint_=Variant::uint_,
		string_=Variant::string_,
		array_,
		struct_
	};

	VariantStruct()						:Variant(){}
	VariantStruct( const Variant& e)			{Variant::initcopy( *this, e);}
	VariantStruct( const StructDescription* d)		{initstruct( d);}
	VariantStruct( const VariantStruct& o)			:Variant(){initcopy( *this, o);}
	VariantStruct array() const;
	~VariantStruct()					{release();}

	VariantStruct& operator=( const VariantStruct& o)	{release(); initcopy( *this, o); return *this;}

public:
	bool operator==( const VariantStruct& o) const		{return compare( o) == 0;}
	bool operator!=( const VariantStruct& o) const		{int cv = compare( o); return cv != 0 && cv != -2;}
	bool operator>( const VariantStruct& o) const		{int cv = compare( o); return cv > 0;}
	bool operator>=( const VariantStruct& o) const		{int cv = compare( o); return cv >= 0;}
	bool operator<=( const VariantStruct& o) const		{int cv = compare( o); return cv <= 0 && cv != -2;}
	bool operator<( const VariantStruct& o) const		{int cv = compare( o); return cv == -1;}

	Type type() const					{return (Type)m_type;}
	const Data& data() const				{return m_data;}
	std::size_t size() const				{return ((Type)m_type==array_)?m_data.dim.size:Variant::size();}
	const StructDescription* description() const		{return ((Type)m_type==struct_)?(const StructDescription*)m_data.dim.metadata:0;}

	std::string tostring() const;
	const VariantStruct& operator[]( std::size_t idx) const;
	VariantStruct& operator[]( std::size_t idx);

private:
	void setType( Type type_)				{m_type = (Variant::Type)type_;}
	int compare( const VariantStruct& o) const;
	static int compare_array( std::size_t size, const VariantStruct* a1, const VariantStruct* a2);
	void initstruct( const StructDescription* description);
	static void initcopy( VariantStruct& dest, const VariantStruct& orig);
	void release();
};


class StructDescription
{
public:
	StructDescription();
	StructDescription( const StructDescription& o);
	void add( const char* name, const VariantStruct& initvalue);

	int find( const char* name);
	VariantStruct* value( VariantStruct* st_, int findidx) const			{return ((std::size_t)findidx >= m_size || findidx < 0)?0:&(*st_)[ findidx];}
	const VariantStruct* value( const VariantStruct* st_, int findidx) const	{return ((std::size_t)findidx >= m_size || findidx < 0)?0:&(*st_)[ findidx];}
	const VariantStruct* prototype( int findidx) const				{return ((std::size_t)findidx >= m_size || findidx < 0)?0:m_initvaluear[ findidx];}
	std::size_t size() const							{return m_size;}

	class const_iterator
	{
	public:
		const_iterator( const StructDescription& visited_)
			:m_visited(&visited_),m_idx(0){}
		const_iterator( const const_iterator& o)
			:m_visited(o.m_visited),m_idx(o.m_idx){}
		const_iterator()
			:m_visited(0),m_idx(0){}

		int compare( const const_iterator& o) const;
		void increment();

		bool operator==( const const_iterator& o) const		{return compare(o) == 0;}
		bool operator!=( const const_iterator& o) const		{return compare(o) != 0;}
		bool operator<( const const_iterator& o) const		{return compare(o) < 0;}
		bool operator<=( const const_iterator& o) const		{return compare(o) <= 0;}
		bool operator>( const const_iterator& o) const		{return compare(o) > 0;}
		bool operator>=( const const_iterator& o) const		{return compare(o) >= 0;}

		const char* name() const				{return (m_visited && m_visited->size() > m_idx)?m_visited->m_namear[ m_idx]:0;}
		std::size_t idx() const					{return m_idx;}
		const VariantStruct* prototype() const			{return (m_visited && m_visited->size() > m_idx)?m_visited->m_initvaluear[ m_idx]:0;}
		const_iterator& operator++()				{increment(); return *this;}
		const_iterator operator++(int)				{const_iterator rt(*this); increment(); return rt;}
		const const_iterator* operator->() const		{return this;}
	private:
		const StructDescription* m_visited;
		std::size_t m_idx;
	};

	const_iterator begin() const					{return const_iterator( *this);}
	const_iterator end() const					{return const_iterator();}

private:
	friend class StructDescription::const_iterator;
	std::size_t m_size;
	char** m_namear;
	VariantStruct** m_initvaluear;
};


}} //namespace
#endif



