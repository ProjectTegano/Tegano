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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\brief Implemention of programs for the database
///\file transactionProgram.cpp
#include "database/transactionProgram.hpp"
#include "utils/miscUtils.hpp"
#include "config/programBase.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

static bool isLineStart( std::string::const_iterator si, const std::string& src)
{
	if (si == src.begin()) return true;
	for (--si; si >= src.begin() && *si <= ' ' && *si > '\0'; --si)
	{
		if (*si == '\n') return true;
	}
	return si == src.begin();
}

static std::string::const_iterator lineStart( std::string::const_iterator si, const std::string& src)
{
	if (si == src.begin()) return si;
	for (--si; si >= src.begin() && *si <= ' ' && *si > '\0'; --si)
	{
		if (*si == '\n') return si+1;
	}
	throw std::logic_error( "internal: Called lineStart without calling isLineStart before");
}

static std::size_t lineCount( std::string::const_iterator si, std::string::const_iterator se)
{
	std::size_t rt = 0;
	for (; si != se; ++si)
	{
		if (*si == '\n') ++rt;
	}
	return rt;
}

const utils::CharTable TransactionProgram::m_optab( ";:-,.=)(<>[]{}/&%*|+-#?!$");

static bool isAlphaNumeric( char ch)
{
	if (ch >= '0' && ch <= '9') return true;
	if (ch >= 'A' && ch <= 'Z') return true;
	if (ch >= 'a' && ch <= 'z') return true;
	if (ch == '_') return true;
	return false;
}

static bool checkResultIdentifier( const std::string& id)
{
	if (id == ".") return true;
	std::string::const_iterator ii = id.begin(), ie = id.end();
	while (ii != ie && isAlphaNumeric( *ii)) ++ii;
	return (ii == ie);
}


void TransactionProgram::loadfile( const std::string& filename, std::string& dbsource)
{
	try
	{
		load( utils::readSourceFileContent( filename), dbsource);
	}
	catch (const config::PositionalErrorException& e)
	{
		throw config::PositionalErrorException( filename, e);
	}
}

bool TransactionProgram::is_mine( const std::string& filename) const
{
	return boost::algorithm::to_lower_copy( utils::getFileExtension( filename)) == ".tdl";
}

char TransactionProgram::gotoNextToken( std::string::const_iterator& si, const std::string::const_iterator se) const
{
	const char* commentopr = m_langdescr->eoln_commentopr();
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

char TransactionProgram::parseNextToken( std::string& tok, std::string::const_iterator& si, std::string::const_iterator se) const
{
	char ch = gotoNextToken( si, se);
	if (!ch) return 0;
	return utils::parseNextToken( tok, si, se, m_optab);
}

std::pair<std::string,std::vector<std::string> > TransactionProgram::parseEmbeddedStatement( const std::string& funcname, int index, std::string::const_iterator& osi, std::string::const_iterator ose)
{
	std::pair<std::string,std::vector<std::string> > rt;
	std::string stm;
	std::string dbstm = m_langdescr->parseEmbeddedStatement( osi, ose);
	std::string::const_iterator start = dbstm.begin(), si = dbstm.begin(), se = dbstm.end();
	std::string tok;

	char ch = utils::parseNextToken( tok, si, se, m_optab);
	for (; si != se && ch; ch = utils::parseNextToken( tok, si, se, m_optab))
	{
		if (ch == '$')
		{
			stm.append( start, si - 1);
			start = si - 1;
			ch = utils::gotoNextToken( si, se);
			if (ch == '(')
			{
				++si;
				std::string::const_iterator argstart = si;
				ch = utils::parseNextToken( tok, si, se, m_optab);
				for (; ch && ch != ')'; ch=utils::parseNextToken( tok, si, se, m_optab));
				if (ch == ')')
				{
					rt.second.push_back( std::string( argstart, si-1));
					start = si;
					stm.append( m_langdescr->stm_argument_reference( rt.second.size()));
				}
			}
			else if (ch >= '0' && ch <= '9')
			{
				std::string::const_iterator argstart = si;
				for (; si!=se && *si>= '0' && *si<= '9'; ++si);
				rt.second.push_back( std::string("$") + std::string( argstart, si));
				start = si;
				stm.append( m_langdescr->stm_argument_reference( rt.second.size()));
			}
		}
	}
	stm.append( start, si);

	rt.first.append( "__");
	rt.first.append( funcname);
	rt.first.append( "_");
	rt.first.append( boost::lexical_cast<std::string>( index));
	m_embeddedStatementMap.insert( rt.first, stm);
	return rt;
}

std::pair<std::string,std::vector<std::string> > TransactionProgram::parseCallStatement( std::string::const_iterator& ci, std::string::const_iterator ce) const
{
	std::pair<std::string,std::vector<std::string> > rt;
	std::string tok;
	int brkcnt = 0;

	if (!utils::gotoNextToken( ci, ce))
	{
		throw std::runtime_error( "unexpected end of transaction description. Function call expected");
	}
	while (ci < ce && isAlphaNumeric( *ci))
	{
		rt.first.push_back( *ci);
		++ci;
	}
	if (rt.first.empty())
	{
		throw std::runtime_error( "identifier expected for name of function");
	}
	utils::gotoNextToken( ci, ce);
	if (*ci != '(')
	{
		throw std::runtime_error( "'(' expected after function name");
	}
	++ci; utils::gotoNextToken( ci, ce);

	// Parse parameter list:
	if (*ci == ')')
	{
		// ... empty parameter list
		++ci;
	}
	else
	{
		for (;;)
		{
			std::string pp;
			while (ci < ce && *ci != ',')
			{
				char hh = *ci++;
				if (hh == '(') ++brkcnt;
				if (hh == ')' && --brkcnt < 0)
				{
					--ci;
					brkcnt = 0;
					break;
				}
				pp.push_back( hh);
			}
			if (brkcnt > 0)
			{
				throw std::runtime_error( "() brackets not balanced");
			}
			boost::trim( pp);
			if (pp.empty())
			{
				throw std::runtime_error( "empty element in parameter list");
			}
			rt.second.push_back( pp);

			utils::gotoNextToken( ci, ce);
			if (*ci == ')')
			{
				++ci;
				break;
			}
			else if (*ci == ',')
			{
				++ci; utils::gotoNextToken( ci, ce);
				continue;
			}
		}
	}
	return rt;
}

namespace {
struct Operation
{
	Operation( const std::string& name_, std::string::const_iterator start_, bool isTransaction_)
		:name(name_),start(start_),isTransaction(isTransaction_),embstm_index(0){}
	Operation( const Operation& o)
		:name(o.name),start(o.start),isTransaction(o.isTransaction),descar(o.descar),resultname(o.resultname),callstartar(o.callstartar),embstm_index(o.embstm_index){}

	std::string name;
	std::string::const_iterator start;
	bool isTransaction;
	std::vector<TransactionDescription> descar;
	std::string resultname;
	std::vector<std::string::const_iterator> callstartar;
	int embstm_index;
};
}// anonymous namespace

void TransactionProgram::load( const std::string& source, std::string& dbsource)
{
	char ch;
	std::string tok;
	std::string::const_iterator si = source.begin(), se = source.end();
	std::string::const_iterator tokstart;
	std::string::const_iterator dbi = source.begin();
	Operation* curop = 0;
	std::vector<Operation> operationar;
	types::keymap<TransactionFunctionR> operationmap;
	std::string authorization_function;
	std::string authorization_resource;

	config::PositionalErrorMessageBase ERROR(source);
	config::PositionalErrorMessageBase::Message MSG;

	dbsource.clear();

	if (!m_langdescr) throw std::logic_error( "no database language description defined");
	try
	{
		while ((ch = gotoNextToken( si, se)) != 0)
		{
			tokstart = si;
			ch = parseNextToken( tok, si, se);
			bool enterDefinition = false;
			bool isTransaction = false;
			if (boost::algorithm::iequals( tok, "TRANSACTION") && isLineStart( tokstart, source))
			{
				isTransaction = true;
				enterDefinition = true;
			}
			else if (boost::algorithm::iequals( tok, "OPERATION") && isLineStart( tokstart, source))
			{
				isTransaction = false;
				enterDefinition = true;
			}
			if (enterDefinition)
			{
				std::string::const_iterator dbe = lineStart( tokstart, source);
				std::string transactionName;

				ch = parseNextToken( transactionName, si, se);
				if (!ch) throw ERROR( si, MSG << "unexpected end of transaction definition (transaction name expected)");
				if (m_optab[ ch]) throw ERROR( si, MSG << "identifier (transaction name) expected instead of '" << ch << "'");

				operationar.push_back( Operation( transactionName, dbe, isTransaction));
				curop = &operationar.back();

				dbsource.append( std::string( dbi, dbe));
				dbi = dbe;
				TransactionDescription desc;
				unsigned int mask = 0;

				while (parseNextToken( tok, si, se))
				{
					if (boost::algorithm::iequals( tok, "RESULT"))
					{
						if (!parseNextToken( tok, si, se)
						||  !boost::algorithm::iequals( tok, "INTO"))
						{
							throw ERROR( si, "INTO expected after RESULT");
						}
						if (0==(ch = parseNextToken( curop->resultname, si, se)))
						{
							throw ERROR( si, "unexpected end of description. name of result tag expected after RESULT INTO");
						}
						if (ch == '.') curop->resultname.push_back(ch);
						if (!checkResultIdentifier( curop->resultname))
						{
							throw ERROR( si, "identifier expected after RESULT INTO");
						}
					}
					else if (boost::algorithm::iequals( tok, "AUTHORIZE"))
					{
						if (isTransaction)
						{
							throw ERROR( si, "Cannot define AUTHORIZE in operation. Only allowed as TRANSACTION definition attribute");
						}
						ch = gotoNextToken( si, se);
						if (ch != '(') throw ERROR( si, "Open bracket '(' expected after AUTHORIZE function call");
						si++;
						if (!parseNextToken( authorization_function, si, se))
						{
							throw ERROR( si, "unexpected end of description. function name expected after AUTHORIZE");
						}
						if (authorization_function.empty())
						{
							throw ERROR( si, "AUTHORIZE function name must not be empty");
						}
						ch = gotoNextToken( si, se);
						if (ch == ',')
						{
							++si;
							if (!parseNextToken( authorization_resource, si, se))
							{
								throw ERROR( si, "unexpected end of description. resource name expected as argument of AUTHORIZE function call");
							}
						}
						ch = gotoNextToken( si, se);
						if (ch != ')')
						{
							throw ERROR( si, "Close bracket ')' expected after AUTHORIZE function defintion");
						}
						++si;
					}
					else
					{
						break;
					}
				}
				if (!boost::algorithm::iequals( tok, "BEGIN"))
				{
					throw ERROR( si, "BEGIN (transaction) expected");
				}
				while ((ch = parseNextToken( tok, si, se)) != 0)
				{
					while (curop->callstartar.size() <= curop->descar.size())
					{
						curop->callstartar.push_back( si);
					}
					if (ch == ';')
					{
						if (mask)
						{
							curop->descar.push_back( desc);
							desc.clear();
							mask = 0;
						}
					}
					else if (m_optab[ch])
					{
						throw ERROR( si, MSG << "keyword (END,FOREACH,INTO,DO) expected instead of operator '" << ch << "'");
					}
					else if (ch == '\'' || ch == '\"')
					{
						throw ERROR( si, "keyword (END,FOREACH,INTO,DO) expected instead string");
					}
					else if (boost::algorithm::iequals( tok, "END"))
					{
						break;
					}
					else if (boost::algorithm::iequals( tok, "FOREACH"))
					{
						if (0 != (mask & (1 << (unsigned)TransactionDescription::Selector)))
						{
							throw ERROR( si, "selector (FOREACH ..) specified twice in a transaction description");
						}
						mask |= (1 << (unsigned)TransactionDescription::Selector);

						ch = utils::parseNextToken( desc.selector, si, se, utils::emptyCharTable(), utils::anyCharTable());
						if (!ch) throw ERROR( si, "unexpected end of description. sector path expected after FOREACH");
					}
					else if (boost::algorithm::iequals( tok, "INTO"))
					{
						if (0 != (mask & (1 << (unsigned)TransactionDescription::Output)))
						{
							throw ERROR( si, "function result (INTO ..) specified twice in a transaction description");
						}
						mask |= (1 << (unsigned)TransactionDescription::Output);

						ch = parseNextToken( desc.output, si, se);
						if (!ch) throw ERROR( si, "unexpected end of description. result tag path expected after INTO");
						if (ch == '.') desc.output.push_back(ch);

						if (!checkResultIdentifier( desc.output))
						{
							throw ERROR( si, "identifier expected after RESULT INTO");
						}
					}
					else if (boost::algorithm::iequals( tok, "DO"))
					{
						if (0 != (mask & (1 << (unsigned)TransactionDescription::Call)))
						{
							throw ERROR( si, "function call (DO ..) specified twice in a transaction description");
						}
						mask |= (1 << (unsigned)TransactionDescription::Call);

						std::string::const_iterator oi = si;
						while (parseNextToken( tok, oi, se))
						{
							if (boost::algorithm::iequals( tok, "NONEMPTY"))
							{
								desc.nonempty = true;
								si = oi;
							}
							else if (boost::algorithm::iequals( tok, "UNIQUE"))
							{
								desc.unique = true;
								si = oi;
							}
							else
							{
								break;
							}
						}
						if (!gotoNextToken( si, se))
						{
							throw ERROR( si, "unexpected end of transaction description after DO");
						}
						if (m_langdescr->isEmbeddedStatement( si, se))
						{
							desc.call = parseEmbeddedStatement( transactionName, curop->embstm_index++, si, se);
						}
						else
						{
							desc.call = parseCallStatement( si, se);
						}
					}
					else
					{
						throw ERROR( si, MSG << "keyword (END,FOREACH,INTO,DO) expected instead of '" << tok << "'");
					}
				}
				// append empty lines to keep line info for the dbsource:
				dbsource.append( std::string( lineCount( dbi, si), '\n'));
				dbi = si;
			}
		}
	}
	catch (const config::PositionalErrorException& e)
	{
		throw e;
	}
	catch (const std::runtime_error& e)
	{
		throw ERROR( si, e.what());
	}
	dbsource.append( std::string( dbi, source.end()));

	std::vector<Operation>::const_iterator oi = operationar.begin(), oe = operationar.end();
	for (; oi != oe; ++oi)
	{
		try
		{
			langbind::Authorization auth;
			if (oi->isTransaction)
			{
				if (!authorization_function.empty())
				{
					auth = langbind::Authorization( authorization_function, authorization_resource);
				}
				m_functionmap[ oi->name] = TransactionFunctionR( createTransactionFunction( oi->name, oi->descar, oi->resultname, operationmap, auth));
			}
			else
			{
				operationmap[ oi->name] = TransactionFunctionR( createTransactionFunction( oi->name, oi->descar, oi->resultname, operationmap, auth));
			}
		}
		catch (const TransactionDescription::Error& err)
		{
			std::string errlocation;
			switch (err.elemname)
			{
				case TransactionDescription::Selector:
					errlocation = "in selector expression (FOREACH ..)";
				break;
				case TransactionDescription::Output:
					errlocation = "in transaction ouput (INTO ..)";
				break;
				case TransactionDescription::Call:
					errlocation = "in transaction call (DO ..)";
				break;
			}
			throw ERROR( oi->callstartar[ err.elemidx], MSG << "error in definition of transaction '" << oi->name << "' " << errlocation << ": "<< err.msg);
		}
		catch (const std::runtime_error& err)
		{
			throw ERROR( oi->start, MSG << "error in definition of transaction '" << oi->name << "': " << err.what());
		}
	}
}

const TransactionFunction* TransactionProgram::function( const std::string& name) const
{
	types::keymap<TransactionFunctionR>::const_iterator fi = m_functionmap.find( name);
	if (fi == m_functionmap.end()) return 0;
	return fi->second.get();
}


