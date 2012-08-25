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
///\file types/bcdArithmetic.hpp
///\brief Defines some operations on arbitrary sized packed bcd numbers
///\note for the Addition,Subtraction and Verification we thank Douglas W. Jones for http://www.divms.uiowa.edu/~jones/bcd/bcd.html
#ifndef _Wolframe_TYPES_BCD_ARITHMETIC_HPP_INCLUDED
#define _Wolframe_TYPES_BCD_ARITHMETIC_HPP_INCLUDED
#include <string>
#include <boost/cstdint.hpp>
#include "types/allocators.hpp"

namespace _Wolframe {
namespace types {

#define _Wolframe_TYPES_BCD_USE_64BIT
#ifdef _Wolframe_TYPES_BCD_USE_64BIT
typedef boost::uint64_t BCDElement;
typedef boost::uint64_t FactorType;
#else
typedef boost::uint32_t BCDElement;
typedef boost::uint32_t FactorType;
#endif

class BigBCD
{
public:
	BigBCD();
	BigBCD( const std::string& numstr);
	BigBCD( long num);
	BigBCD( const BigBCD& o);
	~BigBCD();

	void init( const BigBCD& o)			{copy( o, 0);}
	void init( const std::string& str);
	std::string tostring() const;

	BigBCD operator /( const BigBCD& opr) const	{return div( opr);}
	BigBCD operator *( const BigBCD& opr) const	{return mul( opr);}
	BigBCD operator +( const BigBCD& opr) const	{return add( opr);}
	BigBCD operator -( const BigBCD& opr) const	{return sub( opr);}
	BigBCD operator -() const			{return neg();}

	BigBCD shift( int digits) const;
	void invert_sign()				{m_neg = !m_neg; normalize();}
	char sign() const				{return m_neg?'-':'+';}

	bool operator==( const BigBCD& o) const		{return isequal(o);}
	bool operator!=( const BigBCD& o) const		{return !isequal(o);}
	bool operator<=( const BigBCD& o) const		{return isle(o);}
	bool operator<( const BigBCD& o) const		{return islt(o);}
	bool operator>=( const BigBCD& o) const		{return !islt(o);}
	bool operator>( const BigBCD& o) const		{return !isle(o);}

	bool isValid() const;
	bool isNull() const;

	friend class const_iterator;
	class const_iterator
	{
	public:
		const_iterator();
		const_iterator( const BigBCD& bcd);
		const_iterator( const const_iterator& o)	:m_idx(o.m_idx),m_shf(o.m_shf),m_ar(o.m_ar){}
		const_iterator& operator++()			{increment(); return *this;}
		const_iterator operator++(int)			{const_iterator rt(*this); increment(); return rt;}
		unsigned char operator*() const;
		const const_iterator* operator->() const	{return this;}

		char ascii() const				{return operator*() + '0';}
		std::size_t size() const;

		bool operator==( const const_iterator& o) const	{return isequal(o);}
		bool operator!=( const const_iterator& o) const	{return !isequal(o);}
		bool operator<=( const const_iterator& o) const	{return isle(o);}
		bool operator<( const const_iterator& o) const	{return islt(o);}
		bool operator>=( const const_iterator& o) const	{return !islt(o);}
		bool operator>( const const_iterator& o) const	{return !isle(o);}
	private:
		void increment();
		bool isequal( const const_iterator& other) const;
		bool islt( const const_iterator& other) const;
		bool isle( const const_iterator& other) const;

		std::size_t m_idx;
		unsigned char m_shf;
		const BCDElement* m_ar;
	};

	const_iterator begin() const				{return const_iterator(*this);}
	const_iterator end() const				{return const_iterator();}

private:
	typedef GreedySmallChunkAllocator Allocator;
	void init( std::size_t size_, Allocator* allocator=0);
	void copy( const BigBCD& o, Allocator* allocator);
	void normalize();

	static void digits_addition( BigBCD& dest, const BigBCD& this_, const BigBCD& opr, Allocator* allocator);
	static void digits_subtraction( BigBCD& dest, const BigBCD& this_, const BigBCD& opr, Allocator* allocator);
	static void digits_shift( BigBCD& dest, const BigBCD& this_, int nof_digits, Allocator* allocator);
	static void digits_nibble_multiplication( BigBCD& dest, const BigBCD& this_, unsigned char factor, Allocator* allocator);
	static void digits_16_multiplication( BigBCD& dest, const BigBCD& this_, Allocator* allocator);
	static void digits_multiplication( BigBCD& dest, const BigBCD& this_, FactorType factor, Allocator* allocator);
	static void digits_multiplication( BigBCD& dest, const BigBCD& this_, const BigBCD& factor, Allocator* allocator);
	static void digits_division( BigBCD& dest, const BigBCD& this_, const BigBCD& factor, Allocator* allocator);
	static void xchg( BigBCD& a, BigBCD& b);
	static FactorType division_estimate( const BigBCD& this_, const BigBCD& opr);
	static BigBCD estimate_as_bcd( FactorType estimate, int estshift, Allocator* allocator);

	BigBCD add( const BigBCD& opr) const;
	BigBCD sub( const BigBCD& opr) const;
	BigBCD mul( FactorType opr) const;
	BigBCD mul( long opr) const;
	BigBCD mul( const BigBCD& opr) const;
	BigBCD div( const BigBCD& opr) const;
	BigBCD neg() const;

	bool isequal( const BigBCD& other) const;
	bool islt( const BigBCD& other) const;
	bool isle( const BigBCD& other) const;
	bool isabslt( const BigBCD& other) const;
	bool isabsle( const BigBCD& other) const;

private:
	std::size_t m_size;
	BCDElement* m_ar;
	bool m_neg;
	bool m_allocated;
};

class BigNumber :public BigBCD
{
public:
	BigNumber()
		:m_show_precision(0)
		,m_calc_precision(0){}

	BigNumber( const BigNumber& o)
		:BigBCD(o)
		,m_show_precision(o.m_show_precision)
		,m_calc_precision(o.m_calc_precision){}

	BigNumber( const BigBCD& o, unsigned int sp, unsigned int cp)
		:BigBCD(o)
		,m_show_precision(sp)
		,m_calc_precision(cp){}

	BigNumber( const std::string& numstr, unsigned int sp=0, unsigned int cp=0);

	void format( unsigned int show_prec, unsigned int calc_prec);

	std::string tostring() const;

	BigNumber& operator=( const BigNumber& o);

	BigNumber operator /( const BigNumber& opr) const;
	BigNumber operator *( const BigNumber& opr) const;
	BigNumber operator *( unsigned int opr) const;
	BigNumber operator +( const BigNumber& opr) const;
	BigNumber operator -( const BigNumber& opr) const;
	BigNumber operator -() const;

	bool operator==( const BigNumber& o) const		{return isequal(o);}
	bool operator!=( const BigNumber& o) const		{return !isequal(o);}
	bool operator<=( const BigNumber& o) const		{return isle(o);}
	bool operator<( const BigNumber& o) const		{return islt(o);}
	bool operator>=( const BigNumber& o) const		{return !islt(o);}
	bool operator>( const BigNumber& o) const		{return !isle(o);}
private:
	bool isequal( const BigNumber& other) const;
	bool islt( const BigNumber& other) const;
	bool isle( const BigNumber& other) const;

	unsigned int m_show_precision;
	unsigned int m_calc_precision;
};

}}//namespace
#endif
