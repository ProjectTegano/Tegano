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
///\brief Implementation of utility functions for parsing
///\file tdl/parseUtils.cpp
#include "tdl/parseUtils.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;
using namespace _Wolframe::db::tdl;

static const utils::CharTable g_tdl_optab( ";:-,.=)(<>[]{}/&%*|+-#?!$");

bool tdl::isIdentifier( const std::string& str)
{
	std::string::const_iterator si = str.begin(), se = str.end();
	if (si == se) return false;
	if (!isAlpha(*si)) return false;
	for (++si; si != se && isAlphaNumeric(*si); ++si){}
	return (si == se);
}

std::string tdl::extractImplicitNameFromSelector( const std::string& selector)
{
	if (selector == ".")
	{
		return std::string();
	}
	std::string::const_iterator si = selector.begin(), se = selector.end();
	std::string::const_iterator lastElemStart = si;
	for (; si != se; ++si)
	{
		if (*si == '/') lastElemStart = si+1;
	}
	return std::string( lastElemStart, se);
}

std::string tdl::errorTokenString( char ch, const std::string& tok)
{
	if (g_tdl_optab[ch])
	{
		std::string rt;
		rt.push_back( ch);
		return rt;
	}
	if (ch == '"' || ch == '\'')
	{
		std::string rt( "string ");
		rt.push_back( ch);
		rt.append( tok);
		rt.push_back( ch);
		return rt;
	}
	if (ch == 0)
	{
		return "end of file";
	}
	return tok;
}

char tdl::gotoNextToken( std::string::const_iterator& si, const std::string::const_iterator se)
{
	const char* commentopr = "--";

	char ch;
	while ((ch = utils::gotoNextToken( si, se)) != 0)
	{
		if (ch == commentopr[0])
		{
			std::string::const_iterator ti = si+1;
			std::size_t ci = 1;
			while (commentopr[ci] && ti != se && commentopr[ci] == *ti)
			{
				++ci;
				++ti;
			}
			if (!commentopr[ci])
			{
				// skip to end of line
				while (ti != se && *ti != '\n') ++ti;
				si = ti;
				continue;
			}
		}
		break;
	}
	return ch;
}

char tdl::parseNextToken( std::string& tok, std::string::const_iterator& si, std::string::const_iterator se)
{
	char ch = gotoNextToken( si, se);
	if (!ch) return 0;
	return utils::parseNextToken( tok, si, se, g_tdl_optab);
}

std::vector<std::string> tdl::parse_INTO_path( std::string::const_iterator& si, std::string::const_iterator se)
{
	std::vector<std::string> rt;
	for (;;)
	{
		std::string output;
		char ch = parseNextToken( output, si, se);
		if (!ch) throw std::runtime_error( "unexpected end of description. result tag path expected after INTO");
		if (ch == '.' && output.empty()) output.push_back(ch);
		if (output.empty()) throw std::runtime_error( "identifier or '.' expected after INTO");
		if (!rt.empty() && rt.back() == ".") rt.pop_back();
		rt.push_back( output);
		ch = gotoNextToken( si, se);
		if (ch == '.') throw std::runtime_error( "unexpected dot '.' in INPUT PATH only single dot allowed");
		if (ch != '/') break;
		++si;
	}
	return rt;
}

static std::vector<std::string> parseArguments( const char* stmname, char sb, char eb, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	std::vector<std::string> rt;
	std::string tok;
	if (gotoNextToken( si, se) == sb)
	{
		++si;
		for (;;)
		{
			char ch = parseNextToken( tok, si, se);
			if (!ch) throw std::runtime_error( std::string("unexpected end of ") + stmname + " argument list");
			if (ch == ',') std::runtime_error( std::string("expected ") + stmname + " argument identifier before comma ','");
			if (ch == eb) break;
			if (!isAlpha(ch)) throw std::runtime_error( std::string( "argument is not an identifier: '") + errorTokenString( ch, tok) + "'");
			rt.push_back( tok);
			ch = parseNextToken( tok, si, se);
			if (ch == eb) break;
			if (ch == ',') continue;
			if (ch == ',') std::runtime_error( std::string( "expected comma ',' or end of ") + stmname + " identifier list '" + eb + "'");
		}
	}
	return rt;
}

std::vector<std::string> tdl::parseTemplateArguments( std::string::const_iterator& si, const std::string::const_iterator& se)
{
	return parseArguments( "template", '<', '>', si, se);
}

std::vector<std::string> tdl::parseCallArguments( std::string::const_iterator& si, const std::string::const_iterator& se)
{
	return parseArguments( "call", '(', ')', si, se);
}

void tdl::checkUniqOccurrence( int id, unsigned int& mask, const utils::IdentifierTable& idtab)
{
	unsigned int idshft = (1 << (id+1));
	if (0 != (mask & idshft))
	{
		throw std::runtime_error( std::string( idtab.idstring(id)) + " specified twice in a command");
	}
	mask |= idshft;
}

static const utils::CharTable g_function_optab( ",)(");
static const utils::CharTable g_function_idtab( "a..zA..Z0..9_.");

std::string tdl::parseFunctionName( std::string::const_iterator& si, std::string::const_iterator se)
{
	std::string rt;
	char ch = gotoNextToken( si, se);
	if (!ch)
	{
		throw std::runtime_error( "unexpected end of transaction (function name expected)");
	}
	if (!isAlphaNumeric( ch))
	{
		throw std::runtime_error( "unexpected token (function name expected)");
	}
	(void)utils::parseNextToken( rt, si, se, g_function_optab, g_function_idtab);
	return rt;
}

std::string tdl::parseResourceName( std::string::const_iterator& si, std::string::const_iterator se)
{
	return parseFunctionName( si, se);
}


static const utils::CharTable g_path_optab( ";,)(", false);
static const utils::CharTable g_path_idtab( ";,)(", true);

std::string tdl::parseSelectorPath( std::string::const_iterator& si, std::string::const_iterator se)
{
	std::string rt;
	(void)gotoNextToken( si, se);
	char ch = utils::parseNextToken( rt, si, se, g_path_optab, g_path_idtab);
	if (!ch) throw std::runtime_error("unexpected end of file (selector path expression expected)");
	if (ch == '\"' || ch == '\'') throw std::runtime_error("unexpected string token (selector path expression expected)");
	return rt;
}

bool tdl::parseKeyword( std::string::const_iterator& si, std::string::const_iterator se, const char* keyword)
{
	std::string::const_iterator start = si;
	std::string tok;
	char ch = parseNextToken( tok, si, se);
	if (isAlpha(ch) && boost::algorithm::iequals( tok, keyword))
	{
		return true;
	}
	si = start;
	return false;
}

std::string tdl::parseFilename( std::string::const_iterator& si, std::string::const_iterator se)
{
	std::string rt;
	(void)gotoNextToken( si, se);
	char ch = utils::parseNextToken( rt, si, se, utils::emptyCharTable(), utils::anyCharTable());
	if (!ch) throw std::runtime_error("unexpected end of file (filename expected)");
	return rt;
}

std::string tdl::parseSubroutineName( std::string::const_iterator& si, std::string::const_iterator se)
{
	std::string rt = parseFunctionName( si, se);
	if (!isIdentifier( rt)) throw std::runtime_error( "identifier expected for transaction function or subroutine name");
	return rt;
}

std::string tdl::parseResultName( std::string::const_iterator& si, std::string::const_iterator se)
{
	return parseSubroutineName( si, se);
}

bool tdl::parseNameAssignment( std::string& name, std::string::const_iterator& si, std::string::const_iterator se)
{
	std::string::const_iterator start = si;
	std::string tok;
	bool nameDefined = false;

	char ch = gotoNextToken( si, se);
	if (isAlphaNumeric( ch) || ch == '.')
	{
		//... try to parse assignment first
		parseNextToken( tok, si, se);
		ch = gotoNextToken( si, se);
		if (ch == '=')
		{
			if (tok != ".")
			{
				name = tok;
			}
			nameDefined = true;
			++si;
			ch = gotoNextToken( si, se);
		}
		else
		{
			//... no assignment found -> fallback
			si = start;
		}
	}
	return nameDefined;
}

