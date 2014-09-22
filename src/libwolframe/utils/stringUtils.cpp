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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file stringUtils.cpp
///\brief Some utility string function implementations.
/// They are mainly boost wrappers that are separated because of some warnings needed to be disabled

#ifdef _WIN32
#pragma warning(disable:4996)
#endif
#include "utils/printFormats.hpp"
#include "utils/stringUtils.hpp"
#include "filter/typedfilter.hpp"
#include <cstring>
#include <sstream>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/range/iterator_range.hpp>

using namespace _Wolframe;
using namespace _Wolframe::utils;

void utils::splitString( std::vector<std::string>& res, const std::string& inp, const char* splitchrs)
{
	res.clear();
	std::vector<std::string> imm;
	boost::split( imm, inp, boost::is_any_of(splitchrs));
	std::vector<std::string>::const_iterator vi=imm.begin(), ve=imm.end();
	for (; vi != ve; ++vi) if (!vi->empty()) res.push_back( *vi);
}

void utils::splitString( std::vector<std::string>& res, std::string::const_iterator begin, std::string::const_iterator end, const char* splitchrs)
{
	res.clear();
	std::vector<std::string> imm;
	typedef boost::iterator_range<std::string::const_iterator> irange;
	irange ir = boost::make_iterator_range( begin, end);
	boost::split( imm, ir, boost::is_any_of(splitchrs));
	std::vector<std::string>::const_iterator vi=imm.begin(), ve=imm.end();
	for (; vi != ve; ++vi) if (!vi->empty()) res.push_back( *vi);
}


static std::string substquot( const std::string& o, const utils::PrintFormat* format)
{
	if (!format->endvalue || !format->endvalue[0] || !format->escvalue || !format->escvalue[0])
	{
		return o;
	}
	if (format->endvalue[1] || format->escvalue[1])
	{
		std::size_t endvaluelen = std::strlen( format->endvalue);
		std::string rt;
		char const* ii = const_cast<char const*>( o.c_str());
		char const* ee = ii+o.size();
		for (; ii+endvaluelen <= ee; ++ii)
		{
			if (*ii != *format->endvalue
			||  0!=std::memcmp( ii, format->endvalue, endvaluelen))
			{
				continue;
			}
			rt.append( format->escvalue);
			rt.append( format->endvalue);
			ii += endvaluelen-1;
		}
		return rt;
	}
	else
	{
		char endch = *format->endvalue;
		char escch = *format->escvalue;
		std::string rt;
		char const* ii = const_cast<char const*>( o.c_str());
		char const* ee = ii+o.size();
		for (; ii != ee; ++ii)
		{
			if (*ii == endch)
			{
				rt.push_back( escch);
			}
			rt.push_back( *ii);
		}
		return rt;
	}
}

struct PrintContext
{
	int taglevel;
	std::string indent;
	langbind::FilterBase::ElementType lasttype;
	types::Variant m_lastarraytag;
	std::size_t m_cnt;

	PrintContext()
		:taglevel(0),lasttype(langbind::FilterBase::CloseTag),m_cnt(0)
	{}
};

static void printElement( std::ostream& dest, langbind::FilterBase::ElementType type, const types::VariantConst& element, const utils::PrintFormat* format, PrintContext& ctx)
{
	if (ctx.taglevel < 0)
	{
		throw std::runtime_error("illegal print operation in tostring filter (print atfter final close)");
	}
	switch (type)
	{
		case langbind::FilterBase::OpenTagArray:
			++ctx.m_cnt;
			if (ctx.m_lastarraytag != element)
			{
				ctx.m_cnt = 1;
			}
			/*no break here!*/
		case langbind::FilterBase::OpenTag:
			++ctx.taglevel;
			dest << format->newitem;
			if (format->indent)
			{
				dest << ctx.indent;
				ctx.indent.append( format->indent);
			}
			dest << element.tostring();
			if (type == langbind::FilterBase::OpenTagArray)
			{
				dest << "[" << ctx.m_cnt << "]";
			}
			dest << format->openstruct;
			ctx.lasttype = type;
			break;
		case langbind::FilterBase::CloseTag:
			--ctx.taglevel;
			if (ctx.taglevel < 0)
			{
				break;
			}
			if (format->indent)
			{
				std::size_t indentsize = std::strlen(format->indent);
				if (ctx.indent.size() >= indentsize)
				{
					ctx.indent.resize( ctx.indent.size() - indentsize);
				}
			}
			dest << format->closestruct;
			ctx.lasttype = type;
			break;
		case langbind::FilterBase::Attribute:
			dest << format->newitem;
			dest << ctx.indent;
			dest << element.tostring();
			ctx.lasttype = type;
			break;
		case langbind::FilterBase::Value:
			if (ctx.lasttype == langbind::FilterBase::Attribute)
			{
				dest << format->assign;
				if (format->startvalue) dest << format->startvalue;
				dest << substquot( element.tostring(), format);
				if (format->endvalue) dest << format->endvalue;
				ctx.lasttype = langbind::FilterBase::OpenTag;
			}
			else
			{
				dest << format->newitem;
				dest << ctx.indent;
				if (format->startvalue) dest << format->startvalue;
				dest << substquot( element.tostring(), format);
				if (format->endvalue) dest << format->endvalue;
				ctx.lasttype = type;
			}
			break;
	}
}


bool utils::printFilterToStream( std::ostream& dest, langbind::TypedInputFilter& flt, const PrintFormat* format)
{
	PrintContext ctx;
	langbind::FilterBase::ElementType type;
	types::VariantConst element;
	while (flt.getNext( type, element))
	{
		printElement( dest, type, element, format, ctx);
		if (ctx.taglevel < 0) return true;
	}
	return false;
}

std::string utils::filterToString( langbind::TypedInputFilter& flt, const PrintFormat* format)
{
	std::stringstream out;
	if (!printFilterToStream( out, flt, format))
	{
		const char* err = flt.getError();
		if (err)
		{
			throw std::runtime_error(std::string("error iterating on filter to string: ") + err);
		}
		else
		{
			throw std::runtime_error( "filter to string not possible for filter on incomplete content");
		}
	}
	return out.str();
}


