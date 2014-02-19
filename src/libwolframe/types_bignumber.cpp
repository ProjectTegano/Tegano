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
//\file types_bignumber.cpp
//\brief Big number values implementation
#include "types/bignumber.hpp"
#include "utils/conversions.hpp"
#include <limits>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::types;

_WOLFRAME_UINTEGER BigNumber::touint() const
{
	if (m_sign) throw std::runtime_error( "conversion error: negative bcd number to unsigned integer");
	if (m_scale) throw std::runtime_error("conversion error: bcd number with fraction to unsigned integer");
	_WOLFRAME_UINTEGER rt = 0;
	_WOLFRAME_UINTEGER prev = 0;
	unsigned short ii = 0;
	while (ii < m_size)
	{
		rt = rt * 10 + m_ar[ii];
		if (!(rt > prev))
		{
			throw std::runtime_error("conversion error: big number value to big to convert to unsigned integer");
		}
		prev = rt;
	}
	return rt;
}

_WOLFRAME_INTEGER BigNumber::toint() const
{
	_WOLFRAME_INTEGER val = (_WOLFRAME_UINTEGER)touint();
	if (val < 0) throw std::runtime_error("conversion error: big number value to big to convert to signed integer");
	return m_sign?-val:val;
}

double BigNumber::todouble() const
{
	short si = m_scale;
	double rt = 0;
	double prev = 0;
	unsigned short ii = 0;
	while (ii < m_size)
	{
		rt = rt * 10 + m_ar[ii];
		if (!(rt > prev))
		{
			//... catch NaN
			throw std::runtime_error("conversion error: bcd number value to big to convert to double");
		}
		if (si > 0)
		{
			--si;
			rt /= 10;
		}
		prev = rt;
	}
	while (si < 0)
	{
		++si;
		rt *= 10;
		if (!(rt > prev))
		{
			//... catch NaN
			throw std::runtime_error("conversion error: bcd number value out of range");
		}
		prev = rt;
	}
	return rt;
}

static std::string numtostring( bool sign, const unsigned char* digits, int numsize, int numscale, int exponent)
{
	if (numsize == 0) return "0";
	std::string rt;

	if (sign) rt.push_back( '-');
	if (numscale == 0)
	{
		for (int ii=0; ii<numsize; ++ii)
		{
			rt.push_back( digits[ ii] + '0');
		}
		return rt;
	}
	if (numscale > 0)
	{
		if (numscale >= numsize)
		{
			int ii,nof_leading_zeros = numscale - numsize;
			rt.push_back( '0');
			rt.push_back( '.');
			for (ii=0; ii<nof_leading_zeros; ++ii)
			{
				rt.push_back( '0');
			}
			for (ii=0; ii<numsize; ++ii)
			{
				rt.push_back( digits[ ii] + '0');
			}
		}
		else
		{
			int ii,pre_comma_digits = numsize - numscale;
			for (ii=0; ii<pre_comma_digits; ++ii)
			{
				rt.push_back( digits[ ii] + '0');
			}
			rt.push_back( '.');
			for (ii=0; ii<numscale; ++ii)
			{
				rt.push_back( digits[ pre_comma_digits+ii] + '0');
			}
		}
	}
	else
	{
		int ii;
		for (ii=0; ii<numsize; ++ii)
		{
			rt.push_back( digits[ ii] + '0');
		}
		for (ii=0; ii<-numscale; ++ii)
		{
			rt.push_back( digits[ ii] + '0');
		}
	}
	if (exponent)
	{
		rt.push_back( 'E');
		rt.append( utils::tostring_cast( (_WOLFRAME_INTEGER)exponent));
	}
	return rt;
}

std::string BigNumber::tostring() const
{
	return numtostring( m_sign, m_ar, m_size, m_scale, 0);
}

std::string BigNumber::tostringNormalized() const
{
	int numscale = m_scale;
	int numsize = m_size;
	int exponent = 0;

	if (numscale < 0 && -numscale > numsize)
	{
		exponent = -numscale + numsize;
		numscale = numsize;
	}
	else if (numscale > numsize)
	{
		exponent = -(numscale - numsize);
		numscale = numsize;
	}
	return numtostring( m_sign, m_ar, numsize, numscale, exponent);
}

BigNumber::~BigNumber()
{
	if (m_ar && !m_const) std::free( m_ar);
}

BigNumber::BigNumber()
	:m_scale(0),m_sign(false),m_const(false),m_size(0),m_ar(0)
{}

BigNumber::BigNumber( double val)
	:m_scale(0),m_sign(false),m_const(false),m_size(0),m_ar(0)
{
	constructor( boost::lexical_cast<std::string>( val));
}

BigNumber::BigNumber( _WOLFRAME_INTEGER val)
	:m_scale(0),m_sign(false),m_const(false),m_size(0),m_ar(0)
{
	constructor( utils::tostring_cast( val));
}

BigNumber::BigNumber( _WOLFRAME_UINTEGER val)
	:m_scale(0),m_sign(false),m_const(false),m_size(0),m_ar(0)
{
	constructor( utils::tostring_cast( val));
}

BigNumber::BigNumber( const std::string& val)
	:m_scale(0),m_sign(false),m_const(false),m_size(0),m_ar(0)
{
	constructor( val);
}

BigNumber::BigNumber( bool sign_,unsigned short precision_, signed short scale_, const unsigned char* digits_)
	:m_scale(scale_),m_sign(sign_),m_const(false),m_size(precision_),m_ar(0)
{
	m_ar = (unsigned char*)std::malloc( m_size);
	if (!m_ar) throw std::bad_alloc();
	std::memcpy( m_ar, digits_, m_size);
}

BigNumber::BigNumber( const BigNumber& o)
	:m_scale(o.m_scale),m_sign(o.m_sign),m_const(false),m_size(o.m_size),m_ar(o.m_ar)
{
	m_ar = (unsigned char*)std::malloc( m_size);
	if (!m_ar) throw std::bad_alloc();
	std::memcpy( m_ar, o.m_ar, m_size);
}

BigNumber::BigNumber( const ConstQualifier&, bool sign_, unsigned short precision_, signed short scale_, const unsigned char* digits_)
	:m_scale(scale_),m_sign(sign_),m_const(true),m_size(precision_),m_ar(const_cast<unsigned char*>(digits_))
{}

void BigNumber::constructor( const std::string& val)
{
	std::string::const_iterator vi = val.begin(), ve = val.end();
	std::size_t leadingZeros = 0;
	for (; vi != ve && *vi == '0'; ++vi,++leadingZeros){}
	std::size_t valSize = val.size() - leadingZeros;
	if (!valSize) return;
	if (valSize > std::numeric_limits<unsigned short>::max()) throw std::bad_alloc();

	m_ar = (unsigned char*)std::calloc( valSize, 1);
	if (!m_ar) throw std::bad_alloc();

	enum State {NUMS,NUM0,NUM1,FRC0,FRC1,EXPE,EXPS,EXP0,EXP1}; //< parsing states
	State state = NUMS;

	short scaleinc = 0;
	short prev_scaleinc = 0;
	bool expsign = false;

	for (; vi != ve; ++vi)
	{
		switch (state)
		{
			case NUMS://sign of number:
				state = NUM0;
				if (*vi == '-')
				{
					m_sign = true;
					continue;
				}
				//...no break here !

			case NUM0://leading zeros:
				if (*vi == '0') continue;
				state = NUM1;
				//...no break here !

			case NUM1://significant pre-decimal digits:
				if (*vi >= '0' && *vi <= '9')
				{
					m_ar[ m_size++] = *vi - '0';
					continue;
				}
				if (*vi == '.')
				{
					if (m_size)
					{
						state = FRC1;
						//.. there are already significant digits, so all following are significant too
					}
					else
					{
						state = FRC0;
						//.. no significant digits found yet, so leading zeros of the fractional part are not significant either
					}
					continue;
				}
				state = EXPE;
				continue;

			case FRC0://leading zeros of fractional part when no significant digits found yet (only influencing scale):
				if (*vi == '0')
				{
					m_scale++;
					continue;
				}
				state = FRC1;
				//...no break here !

			case FRC1://significant digits of fractional part:
				if (*vi >= '0' && *vi <= '9')
				{
					m_ar[ m_size++] = *vi - '0';
					m_scale++;
					continue;
				}
				state = EXPE;
				//...no break here !

			case EXPE://exponent marker:
				if (*vi == ' ') continue;
				if (*vi == 'E')
				{
					state = EXPS;
					continue;
				}
				throw std::runtime_error("syntax error in big number string");

			case EXPS://exponent sign:
				state = EXP0;
				if (*vi == '-')
				{
					expsign = true;
					continue;
				}
				//...no break here !

			case EXP0://leading zeros of the exponent (ignored):
				if (*vi == '0') continue;
				state = EXP1;
				//...no break here !

			case EXP1://exponent:
				if (*vi >= '0' && *vi <= '9')
				{
					scaleinc = scaleinc * 10 + *vi - '0';
					if (prev_scaleinc > scaleinc) throw std::runtime_error("conversion error: big number value in string is out of range");
					prev_scaleinc = scaleinc;
					continue;
				}
				throw std::runtime_error("syntax error in big number string");
		}
	}
	if (expsign)
	{
		short prev_scale = m_scale;
		m_scale += scaleinc;
		if (prev_scale > m_scale) throw std::runtime_error("conversion error: big number value in string is out of range");
	}
	else
	{
		short prev_scale = m_scale;
		m_scale -= scaleinc;
		if (prev_scale < m_scale) throw std::runtime_error("conversion error: big number value in string is out of range");
	}
	if (m_size == 0)
	{
		m_scale = 0;
		m_sign = 0;
	}
	unsigned char* ar_ = (unsigned char*)std::realloc( m_ar, m_size);
	if (ar_) m_ar = ar_;
}

bool BigNumber::isvalid() const
{
	if (!m_size)
	{
		if (m_scale) return false;
		if (m_sign) return false;
	}
	for (unsigned short ii=0; ii<m_size; ++ii)
	{
		if (m_ar[ii] > 9) return false;
	}
	return true;
}


