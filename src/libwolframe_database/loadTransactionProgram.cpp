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
///\brief Loading function for transaction definition programs
///\file loadTransactionProgram.cpp
#include "database/loadTransactionProgram.hpp"
#include "transactionFunctionDescription.hpp"
#include "utils/parseUtils.hpp"
#include "utils/fileUtils.hpp"
#include "types/conversions.hpp"
#include "logger-v1.hpp"
#include "config/programBase.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

static std::string::const_iterator lineStart( std::string::const_iterator si, const std::string& src)
{
	if (si == src.begin()) return si;
	for (--si; si >= src.begin() && *si <= ' ' && *si > '\0'; --si)
	{
		if (*si == '\n') return si+1;
	}
	throw std::logic_error( "internal: Called lineStart without calling isLineStart before");
}

static const utils::CharTable g_optab( ";:-,.=)(<>[]{}/&%*|+-#?!$");

static bool isAlpha( char ch)
{
	if (ch >= 'A' && ch <= 'Z') return true;
	if (ch >= 'a' && ch <= 'z') return true;
	if (ch == '_') return true;
	return false;
}

static bool isAlphaNumeric( char ch)
{
	if (ch >= '0' && ch <= '9') return true;
	return isAlpha( ch);
}

static bool isIdentifier( const std::string& str)
{
	std::string::const_iterator si = str.begin(), se = str.end();
	if (si == se) return false;
	if (!isAlpha(*si)) return false;
	for (++si; si != se && isAlphaNumeric(*si); ++si){}
	return (si == se);
}

static std::string errorTokenString( char ch, const std::string& tok)
{
	if (g_optab[ch])
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

static char gotoNextToken( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator se)
{
	const char* commentopr = langdescr->eoln_commentopr();
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

static char parseNextToken( const LanguageDescription* langdescr, std::string& tok, std::string::const_iterator& si, std::string::const_iterator se)
{
	char ch = gotoNextToken( langdescr, si, se);
	if (!ch) return 0;
	return utils::parseNextToken( tok, si, se, g_optab);
}

static std::vector<std::string> parse_INTO_path( const LanguageDescription* langdescr, std::string::const_iterator& si, std::string::const_iterator se)
{
	std::vector<std::string> rt;
	for (;;)
	{
		std::string output;
		char ch = parseNextToken( langdescr, output, si, se);
		if (!ch) throw std::runtime_error( "unexpected end of description. result tag path expected after INTO");
		if (ch == '.' && output.empty()) output.push_back(ch);

		if (output.empty())
		{
			throw std::runtime_error( "identifier or '.' expected after INTO");
		}
		rt.push_back( output);
		ch = gotoNextToken( langdescr, si, se);
		if (ch != '/') break;
		++si;
	}
	return rt;
}

static int getResultNamespaceIdentifier( const std::string& name, const types::keymap<int>& keepResult_map, int fidx)
{
	types::keymap<int>::const_iterator ki = keepResult_map.find( name);
	if (ki == keepResult_map.end())
	{
		if (boost::algorithm::iequals( name, "PARAM"))
		{
			return 0;
		}
		else if (boost::algorithm::iequals( name, "RESULT"))
		{
			if (fidx == 0)
			{
				throw std::runtime_error( "no command result referenceable here with RESULT");
			}
			return fidx;
		}
		return -1;
	}
	else
	{
		return ki->second;
	}
}

static std::vector<std::string>
	parseTemplateArguments( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	std::vector<std::string> rt;
	std::string tok;
	if (gotoNextToken( langdescr, si, se) == '<')
	{
		++si;
		for (;;)
		{
			char ch = parseNextToken( langdescr, tok, si, se);
			if (!ch) throw std::runtime_error( "unexpected end of template argument list");
			if (ch == ',') std::runtime_error( "expected template argument identifier before comma ','");
			if (ch == '>') break;
			if (!isAlpha(ch)) throw std::runtime_error( std::string( "template argument is not an identifier: '") + errorTokenString( ch, tok) + "'");
			rt.push_back( tok);
			ch = parseNextToken( langdescr, tok, si, se);
			if (ch == '>') break;
			if (ch == ',') continue;
			if (ch == ',') std::runtime_error( "expected comma ',' or end of template identifier list '>'");
		}
	}
	return rt;
}

static TransactionFunctionDescription::MainProcessingStep::Call::Param
	parseReferenceParameter( const LanguageDescription* langdescr, const types::keymap<int>& param_map, const types::keymap<int>& keepResult_map, int fidx, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	typedef TransactionFunctionDescription::MainProcessingStep::Call Call;
	char ch = utils::gotoNextToken( si, se);
	if (ch == '(')
	{
		char sb,eb;
		Call::Param::Type type;
		sb = '('; eb = ')';
		type = Call::Param::InputSelectorPath;
		++si;
		std::string::const_iterator argstart = si;
		std::string tok;
		ch = parseNextToken( langdescr, tok, si, se);
		for (; ch && ch != sb && ch != eb; ch=parseNextToken( langdescr, tok, si, se));
		if (ch == eb)
		{
			Call::Param param( type, boost::algorithm::trim_copy( std::string( argstart, si-1)));
			return param;
		}
		else
		{
			throw std::runtime_error( std::string( "missing close bracket for expression") + ((eb==')')?"')'":"']'"));
		}
	}
	else if (ch == '#')
	{
		// Internal LOOPCOUNT variable:
		++si;
		Call::Param param( Call::Param::InternalVariable, "#");
		return param;
	}
	else if (isAlphaNumeric(ch))
	{
		// Parse namespaces and result references:
		int resultscope_functionidx = -1;
		if (isAlpha(ch))
		{
			std::string::const_iterator argstart = si;
			for (; si!=se && isAlphaNumeric(*si); ++si);
			if (*si == '.')
			{
				std::string namspace( argstart, si);
				resultscope_functionidx = getResultNamespaceIdentifier( namspace, keepResult_map, fidx);
				if (resultscope_functionidx == -1)
				{
					throw std::runtime_error( std::string( "result set identifier not found '") + namspace + "'");
				}
				++si;
				ch = (si==se)?0:*si;
			}
			else
			{
				si = argstart;
			}
		}
		if (ch >= '0' && ch <= '9')
		{
			std::string::const_iterator argstart = si;
			for (; si!=se && *si>= '0' && *si<= '9'; ++si);
			if (si != se && isAlphaNumeric(*si))
			{
				throw std::runtime_error( "operator or space expected after numeric reference");
			}
			Call::Param::Type type = Call::Param::NumericResultReference;
			Call::Param param( type, std::string( argstart, si), resultscope_functionidx);
			return param;
		}
		else if (isAlpha(ch))
		{
			std::string::const_iterator argstart = si;
			for (; si!=se && isAlphaNumeric(*si); ++si);
			if (resultscope_functionidx == 0)
			{
				// ... Parameter reference has to be converted to a numeric reference
				std::string paramname = std::string( argstart, si);
				types::keymap<int>::const_iterator pi = param_map.find( paramname);
				if (pi == param_map.end())
				{
					throw std::runtime_error( std::string("unknown parameter '") + paramname + "'");
				}
				Call::Param::Type type = Call::Param::NumericResultReference;
				Call::Param param( type, types::tostring_cast((_WOLFRAME_UINTEGER)pi->second), resultscope_functionidx);
				return param;
			}
			else
			{
				Call::Param::Type type = Call::Param::SymbolicResultReference;
				Call::Param param( type, std::string( argstart, si), resultscope_functionidx);
				return param;
			}
		}
		else
		{
			throw std::runtime_error( "expected open bracket '(' or alphanumeric result reference");
		}
	}
	else
	{
		throw std::runtime_error( "expected open bracket '(' or alphanumeric result reference");
	}
}

static TransactionFunctionDescription::MainProcessingStep::Call::Param
	parsePathParameter( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	typedef TransactionFunctionDescription::MainProcessingStep::Call Call;
	std::string pp;
	std::string tok;
	char ch;
	for (;;)
	{
		ch = gotoNextToken( langdescr, si, se);
		if (ch == ',' || ch == ')') break;
		if (!ch) throw std::runtime_error( "unexpected end of function call");

		ch = parseNextToken( langdescr, tok, si, se);

		if (ch == '/' || ch == '*' || ch == '.')
		{
			pp.push_back( ch);
		}
		else if (isAlphaNumeric( ch))
		{
			pp.append( tok);
		}
		else
		{
			throw std::runtime_error( std::string( "illegal token in path parameter of function call: ") + errorTokenString( ch, tok));
		}
	}
	Call::Param param( Call::Param::InputSelectorPath, pp);
	return param;
}

static TransactionFunctionDescription::MainProcessingStep::Call
	parseEmbeddedStatement( const LanguageDescription* langdescr, std::string::const_iterator& osi, std::string::const_iterator ose, const types::keymap<int>& param_map, const types::keymap<int>& keepResult_map, int fidx)
{
	typedef TransactionFunctionDescription::MainProcessingStep::Call Call;
	std::vector<Call::Param> paramlist;	//< list of arguments
	std::string stm;
	std::string dbstm = langdescr->parseEmbeddedStatement( osi, ose);
	std::string::const_iterator start = dbstm.begin(), si = dbstm.begin(), se = dbstm.end();
	std::string tok;

	char ch = parseNextToken( langdescr, tok, si, se);
	for (; si != se && ch; ch = parseNextToken( langdescr, tok, si, se))
	{
		if (ch == '$' && si != se)
		{
			if (*si == '(' || isAlphaNumeric(*si) || *si == '#')
			{
				stm.append( start, si - 1);
				Call::Param param = parseReferenceParameter( langdescr, param_map, keepResult_map, fidx, si, se);
				paramlist.push_back( param);
				start = si;
				stm.append( langdescr->stm_argument_reference( paramlist.size()));
			}
		}
	}
	stm.append( start, si);
	return Call( stm, std::vector<std::string>(), paramlist, true);
}

static TransactionFunctionDescription::MainProcessingStep::Call
	parseCallStatement( const LanguageDescription* langdescr, std::string::const_iterator& ci, std::string::const_iterator ce, const types::keymap<int>& param_map, const types::keymap<int>& keepResult_map, int fidx)
{
	typedef TransactionFunctionDescription::MainProcessingStep::Call Call;
	std::string callname;			//< function name
	std::vector<std::string> templatearg;	//< list of template arguments
	std::vector<Call::Param> paramlist;	//< list of arguments
	std::string tok;

	if (!utils::gotoNextToken( ci, ce))
	{
		throw std::runtime_error( "unexpected end of transaction description. Function call expected");
	}
	while (ci < ce && (isAlphaNumeric( *ci) || *ci == '.'))
	{
		callname.push_back( *ci);
		++ci;
	}
	if (callname.empty())
	{
		throw std::runtime_error( "identifier expected for name of function");
	}
	char ch = utils::gotoNextToken( ci, ce);
	if (ch == '<')
	{
		templatearg = parseTemplateArguments( langdescr, ci, ce);
		ch = utils::gotoNextToken( ci, ce);
	}
	if (ch != '(')
	{
		throw std::runtime_error( "'(' expected after function name");
	}
	++ci; ch = utils::gotoNextToken( ci, ce);

	// Parse parameter list:
	if (ch == ')')
	{
		// ... empty parameter list
		++ci;
	}
	else
	{
		std::string pp;
		ch = ',';
		while (ch == ',')
		{
			ch = gotoNextToken( langdescr, ci, ce);
			if (ch == '\'' || ch == '\"')
			{
				ch = parseNextToken( langdescr, tok, ci, ce);
				Call::Param::Type type = Call::Param::Constant;
				Call::Param param( type, tok);
				paramlist.push_back( param);
			}
			else if (ch == '$')
			{
				++ci;
				Call::Param param = parseReferenceParameter( langdescr, param_map, keepResult_map, fidx, ci, ce);
				paramlist.push_back( param);
			}
			else if (ch == ')' || ch == ',')
			{
				throw std::runtime_error( std::string("parameter expected before '") + (char)ch + "'");
			}
			else
			{
				Call::Param param = parsePathParameter( langdescr, ci, ce);
				paramlist.push_back( param);
			}
			ch = parseNextToken( langdescr, tok, ci, ce);
		}
		if (ch != ')')
		{
			throw std::runtime_error( "unexpected token in function call parameter. close bracket ')' or comma ',' expected after argument");
		}
	}
	return Call( callname, templatearg, paramlist, false);
}

static TransactionFunctionDescription::PreProcessingStep::Argument
	parsePreProcessingStepArgument( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	typedef TransactionFunctionDescription::PreProcessingStep::Argument Argument;
	Argument rt;
	std::string tok;
	char ch;
	bool nameDefined = false;
	bool nameLegal = true;

	ch = gotoNextToken( langdescr, si, se);
	if (isAlphaNumeric( ch) || ch == '.')
	{
		parseNextToken( langdescr, tok, si, se);
		ch = gotoNextToken( langdescr, si, se);
		if (ch == '=')
		{
			nameDefined = true;
			rt.name = tok;
			++si;
			ch = gotoNextToken( langdescr, si, se);
		}
		else
		{
			rt.value.append( tok);
			rt.name = tok;
		}
	}
	if (ch == '\'' || ch == '\"')
	{
		parseNextToken( langdescr, rt.value, si, se);
		rt.type = Argument::Constant;
	}
	else
	{
		rt.type = Argument::Selector;
		for (;;)
		{
			ch = gotoNextToken( langdescr, si, se);
			if (ch == ',' || ch == ')') break;
			if (!ch) throw std::runtime_error( "unexpected end of function call");

			ch = parseNextToken( langdescr, tok, si, se);

			if (ch == '/' || ch == '*' || ch == '.')
			{
				if (ch == '.' && !nameDefined)
				{
					rt.name = ".";
				}
				else
				{
					nameLegal = false;
				}
				rt.value.push_back( ch);
			}
			else if (isAlphaNumeric( ch))
			{
				nameLegal = true;
				if (!nameDefined) rt.name = tok;
				rt.value.append( tok);
			}
			else
			{
				throw std::runtime_error( "illegal token in path parameter of preprocesing command");
			}
		}
		if (!nameDefined && !nameLegal)
		{
			throw std::runtime_error( "illegal token in path parameter of preprocesing command");
		}
		if (rt.name == ".")
		{
			rt.name.clear();
		}
	}
	return rt;
}

static std::vector<TransactionFunctionDescription::PreProcessingStep::Argument>
	parsePreProcessingStepArguments( const LanguageDescription* langdescr, std::string::const_iterator& si, std::string::const_iterator se)
{
	typedef TransactionFunctionDescription::PreProcessingStep::Argument Argument;
	std::vector<Argument> rt;

	for (;;)
	{
		char ch = gotoNextToken( langdescr, si, se);
		if (ch == ')' || ch == '\0') break;

		rt.push_back( parsePreProcessingStepArgument( langdescr, si, se));
	}
	return rt;
}

static void parseSubroutineArguments( types::keymap<int>& param_map, const LanguageDescription* langdescr, std::string::const_iterator& si, std::string::const_iterator se)
{
	std::string varname;
	char ch;
	int column_idx = 0;
	ch = parseNextToken( langdescr, varname, si, se);
	if (ch == ')') return;

	if (isAlphaNumeric(ch))
	{
		types::keymap<int>::const_iterator vi = param_map.find( varname);
		if (vi != param_map.end())
		{
			throw std::runtime_error( std::string("duplicate definition of subroutine parameter '") + varname + "'");
		}
		param_map[ varname] = ++column_idx;
	}
	else
	{
		throw std::runtime_error( "variable name expected (itentifier in SUBROUTINE argument list)");
	}
	while (0!=(ch=gotoNextToken( langdescr, si, se)))
	{
		if (ch != ',') break;
		++si;
		ch = parseNextToken( langdescr, varname, si, se);
		if (isAlphaNumeric(ch))
		{
			types::keymap<int>::const_iterator vi = param_map.find( varname);
			if (vi != param_map.end())
			{
				throw std::runtime_error( std::string("duplicate definition of subroutine parameter '") + varname + "'");
			}
			param_map[ varname] = ++column_idx;
		}
		else
		{
			throw std::runtime_error( "variable name expected (itentifier in SUBROUTINE argument list)");
		}
	}
	if (ch == ')')
	{
		++si;
	}
	else
	{
		throw std::runtime_error( "syntax error in in SUBROUTINE argument list: expected ')'");
	}
}


namespace {
struct Subroutine
{
	Subroutine( const std::vector<std::string>& templateArguments_, std::string::const_iterator start_, bool isTransaction_)
		:templateArguments(templateArguments_),start(start_),isTransaction(isTransaction_),isValidDatabase(true),embstm_index(0){}
	Subroutine( const Subroutine& o)
		:templateArguments(o.templateArguments),start(o.start),isTransaction(o.isTransaction),isValidDatabase(o.isValidDatabase),param_map(o.param_map),description(o.description),callstartar(o.callstartar),pprcstartar(o.pprcstartar),result_INTO(o.result_INTO),blockstk(o.blockstk),embstm_index(o.embstm_index){}

	std::vector<std::string> templateArguments;
	std::string::const_iterator start;
	bool isTransaction;
	bool isValidDatabase;
	types::keymap<int> param_map;
	TransactionFunctionDescription description;
	std::vector<std::string::const_iterator> callstartar;
	std::vector<std::string::const_iterator> pprcstartar;
	std::vector<std::string> result_INTO;					//< RESULT INTO path
	std::vector<TransactionFunctionDescription::ResultBlock> blockstk;	//< stack of current RESULT INTO block scope
	int embstm_index;
};
}// anonymous namespace


static void checkUniqOccurrence( int id, unsigned int& mask, const utils::IdentifierTable& idtab, config::PositionalErrorMessageBase& ERROR, std::string::const_iterator& si)
{
	unsigned int idshft = (1 << (id+1));
	if (0 != (mask & idshft))
	{
		throw ERROR( si, std::string( idtab.idstring(id)) + " specified twice in a command");
	}
	mask |= idshft;
}

static const char* g_preproc_ids[] = {"ENDPROC","FOREACH","INTO","DO",0};
enum PreProcKeyword{ p_NONE,p_ENDPROC,p_FOREACH,p_INTO,p_DO };
static const utils::IdentifierTable g_preproc_idtab( false, g_preproc_ids);

static void parsePreProcessingBlock( Subroutine& subroutine, config::PositionalErrorMessageBase& ERROR, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	config::PositionalErrorMessageBase::Message MSG;
	std::string tok;
	if (!subroutine.isTransaction)
	{
		throw ERROR( si, "Cannot define PREPROC in SUBROUTINE. Only allowed in TRANSACTION definition");
	}
	char ch = 0;
	unsigned int mask = 0;
	TransactionFunctionDescription::PreProcessingStep prcstep;

	while ((ch = gotoNextToken( langdescr, si, se)) != 0)
	{
		while (subroutine.pprcstartar.size() <= subroutine.description.preprocs.size())
		{
			subroutine.pprcstartar.push_back( si);
		}
		switch ((PreProcKeyword)utils::parseNextIdentifier( si, se, g_preproc_idtab))
		{
			case p_NONE:
				ch = parseNextToken( langdescr, tok, si, se);
				if (ch == ';')
				{
					if (mask)
					{
						subroutine.description.preprocs.push_back( prcstep);
						prcstep.clear();
						mask = 0;
					}
				}
				else if (g_optab[ch])
				{
					throw ERROR( si, MSG << "keyword (" << g_preproc_idtab.tostring() << ") expected instead of operator '" << ch << "'");
				}
				else if (ch == '\'' || ch == '\"')
				{
					throw ERROR( si, MSG << "keyword (" << g_preproc_idtab.tostring() << ") expected instead of string");
				}
				else
				{
					throw ERROR( si, MSG << "keyword (" << g_preproc_idtab.tostring() << ") expected instead of identifier '" << tok << "'");
				}
				break;

			case p_ENDPROC:
				if (mask != 0)
				{
					throw std::runtime_error( "preprocessing command not terminated with ';'");
				}
				return;
			case p_FOREACH:
				checkUniqOccurrence( p_FOREACH, mask, g_preproc_idtab, ERROR, si);
	
				ch = utils::parseNextToken( prcstep.selector_FOREACH, si, se, utils::emptyCharTable(), utils::anyCharTable());
				if (!ch) throw ERROR( si, "unexpected end of description. sector path expected after FOREACH");
				break;
			case p_INTO:
				checkUniqOccurrence( p_INTO, mask, g_preproc_idtab, ERROR, si);
	
				prcstep.path_INTO = parse_INTO_path( langdescr, si, se);
				break;

			case p_DO:
				checkUniqOccurrence( p_DO, mask, g_preproc_idtab, ERROR, si);
	
				ch = parseNextToken( langdescr, prcstep.functionname, si, se);
				if (!isAlphaNumeric( ch))
				{
					throw std::runtime_error( "identifier expected for preprocessing function name after DO");
				}
				ch = gotoNextToken( langdescr, si, se);
				if (ch != '(')
				{
					throw std::runtime_error( "'(' expected after preprocessing function name");
				}
				++si;
				prcstep.args = parsePreProcessingStepArguments( langdescr, si, se);
				ch = gotoNextToken( langdescr, si, se);
				if (ch != ')')
				{
					throw std::runtime_error( "')' expected after preprocessing function arguments");
				}
				++si;
				break;
		}
	}
}

static void parseResultDirective( Subroutine& subroutine, config::PositionalErrorMessageBase& ERROR, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	config::PositionalErrorMessageBase::Message MSG;
	std::string tok;
	char ch;
	unsigned int mask = 0;

	std::string::const_iterator start = si;
	subroutine.result_INTO.clear();
	while (0!=(ch=parseNextToken( langdescr, tok, si, se)))
	{
		if (isAlpha(ch) && boost::algorithm::iequals( tok, "INTO"))
		{
			if ((mask & 0x2) != 0) throw ERROR( si, "wrong order of definition in RESULT: INTO defined after FILTER");
			if ((mask & 0x1) != 0) throw ERROR( si, "duplicate INTO definition after RESULT");
			mask |= 0x1;
			subroutine.result_INTO = parse_INTO_path( langdescr, si, se);
		}
		else if (isAlpha(ch) && boost::algorithm::iequals( tok, "FILTER"))
		{
			if (!subroutine.isTransaction)
			{
				throw ERROR( si, "Cannot define RESULT FILTER in SUBROUTINE. Only allowed as in TRANSACTION definition");
			}
			if ((mask & 0x2) != 0) throw ERROR( si, "duplicate FILTER definition after RESULT");
			mask |= 0x2;
			ch = gotoNextToken( langdescr, si, se);
			if (!isAlpha(ch))
			{
				throw ERROR( si, "function name expected after RESULT FILTER");
			}
			while (si < se && (isAlphaNumeric( *si) || *si == '.'))
			{
				subroutine.description.resultfilter.push_back( *si);
				++si;
			}
		}
		else if (mask)
		{
			si = start;
			break;
		}
		else
		{
			throw ERROR( si, "INTO or FILTER expected after RESULT");
		}
		start = si;
	}
}

static void parseAuthorizeDirective( Subroutine& subroutine, config::PositionalErrorMessageBase& ERROR, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	config::PositionalErrorMessageBase::Message MSG;
	std::string tok;

	std::string authfunction;
	std::string authresource;

	if (!subroutine.isTransaction)
	{
		throw ERROR( si, "Cannot define AUTHORIZE in SUBROUTINE. Only allowed as TRANSACTION definition attribute");
	}
	char ch = gotoNextToken( langdescr, si, se);
	if (ch != '(') throw ERROR( si, "Open bracket '(' expected after AUTHORIZE function call");
	si++;
	if (!parseNextToken( langdescr, authfunction, si, se))
	{
		throw ERROR( si, "unexpected end of description. function name expected after AUTHORIZE");
	}
	if (authfunction.empty())
	{
		throw ERROR( si, "AUTHORIZE function name must not be empty");
	}
	ch = gotoNextToken( langdescr, si, se);
	if (ch == ',')
	{
		++si;
		if (!parseNextToken( langdescr, authresource, si, se))
		{
			throw ERROR( si, "unexpected end of description. resource name expected as argument of AUTHORIZE function call");
		}
	}
	ch = gotoNextToken( langdescr, si, se);
	if (ch != ')')
	{
		throw ERROR( si, "Close bracket ')' expected after AUTHORIZE function defintion");
	}
	++si;
	subroutine.description.auth.init( authfunction, authresource);
}

static void parsePrintStep( Subroutine& subroutine, const types::keymap<int>& keepResult_map, config::PositionalErrorMessageBase& ERROR, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se, const std::vector<std::string>& print_INTO_path)
{
	typedef TransactionFunctionDescription::MainProcessingStep::Call Call;
	config::PositionalErrorMessageBase::Message MSG;
	std::string tok;

	std::vector<std::string> pt = print_INTO_path;
	Call::Param printarg;

	char ch = gotoNextToken( langdescr, si, se);
	if (ch == '\'' || ch == '\"')
	{
		ch = parseNextToken( langdescr, tok, si, se);
		printarg = Call::Param( Call::Param::Constant, tok);
	}
	else if (ch == '$')
	{
		++si;
		printarg = parseReferenceParameter( langdescr, subroutine.param_map, keepResult_map, subroutine.description.steps.size(), si, se);
		if (printarg.type == Call::Param::InputSelectorPath)
		{
			throw std::runtime_error( "cannot handle input reference as argument of PRINT");
		}
	}
	else
	{
		throw std::runtime_error( "string or result reference expected as argument of PRINT");
	}
	
	ch = gotoNextToken( langdescr, si, se);
	if (isAlphaNumeric(ch))
	{
		if (0!=(parseNextToken( langdescr, tok, si, se))
		&& isAlpha(ch) && boost::algorithm::iequals( tok, "INTO"))
		{
			if (pt.empty())
			{
				pt = parse_INTO_path( langdescr, si, se);
				ch = gotoNextToken( langdescr, si, se);
			}
			else
			{
				throw ERROR( si, "INTO specified twice for 'PRINT'");
			}
		}
		else
		{
			throw ERROR( si, "unexpected token after 'PRINT' and argument");
		}
	}
	if (ch != ';')
	{
		throw ERROR( si, "unexpected token as end of print expression (';' expected)");
	}
	subroutine.description.printsteps[ subroutine.description.steps.size()] = TransactionFunctionDescription::PrintStep( pt, printarg);
}

static void parseErrorHint( Subroutine& subroutine, config::PositionalErrorMessageBase& ERROR, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	config::PositionalErrorMessageBase::Message MSG;
	std::string tok;
	char ch;

	if (0!=(ch=parseNextToken( langdescr, tok, si, se))
	&&  isAlpha(ch) && boost::algorithm::iequals( tok, "ERROR"))
	{
		if (subroutine.description.steps.empty())
		{
			throw ERROR( si, "ON ERROR declaration allowed only after a database call");
		}
		std::string errclass;
		if (!parseNextToken( langdescr, errclass, si, se))
		{
			throw ERROR( si, "error class expected after ON ERROR");
		}
		if (0==(ch=parseNextToken( langdescr, tok, si, se))
		||  !isAlpha(ch)
		||  !boost::algorithm::iequals( tok, "HINT"))
		{
			throw ERROR( si, "keyword HINT expected after ON ERROR <errorclass>");
		}
		std::string errhint;
		if (!parseNextToken( langdescr, errhint, si, se))
		{
			throw ERROR( si, "hint message string expected after ON ERROR <errorclass> HINT");
		}
		if (subroutine.description.steps.back().hints.find( errclass) != subroutine.description.steps.back().hints.end())
		{
			throw ERROR( si, std::string( "Duplicate hint for error class '") + errclass + "' for this database call");
		}
		subroutine.description.steps.back().hints[ errclass] = errhint;

		if (';' != gotoNextToken( langdescr, si, se))
		{
			throw ERROR( si, "';' expected after ON ERROR declaration");
		}
		++si;
	}
	else
	{
		throw ERROR( si, "keyword (ERROR) expected after ON");
	}
}

static void parseKeepAs( types::keymap<int>& keepResult_map, std::size_t commandlistsize, config::PositionalErrorMessageBase& ERROR, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	config::PositionalErrorMessageBase::Message MSG;
	std::string tok;
	char ch;

	if (0!=(ch=parseNextToken( langdescr, tok, si, se))
	&&  isAlpha(ch) && boost::algorithm::iequals( tok, "AS"))
	{
		ch = parseNextToken( langdescr, tok, si, se);
		if (!isAlpha(ch))
		{
			throw ERROR( si, "identifier expected after KEEP AS");
		}
		ch = gotoNextToken( langdescr, si, se);
		if (ch != ';')
		{
			throw ERROR( si, "semicolon ';' expected after KEEP AS <identifier>");
		}
		++si;
		types::keymap<int>::const_iterator ki = keepResult_map.find( tok);
		if (ki != keepResult_map.end())
		{
			throw ERROR( si, std::string("duplicate definition of result set '") + tok + "'");
		}
		if (commandlistsize == 0)
		{
			throw ERROR( si, "no result available to reference here (KEEP AS)");
		}
		keepResult_map.insert( tok, commandlistsize);
	}
}

static const char* g_mainblock_ids[] = {"END","FOREACH","INTO","DO","ON","KEEP","PRINT","RESULT",0};
enum MainBlockKeyword{ m_NONE, m_END,m_FOREACH,m_INTO,m_DO,m_ON,m_KEEP,m_PRINT,m_RESULT};
static const utils::IdentifierTable g_mainblock_idtab(false, g_mainblock_ids);

static void parseMainBlock( Subroutine& subroutine, config::PositionalErrorMessageBase& ERROR, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	config::PositionalErrorMessageBase::Message MSG;
	std::string tok;
	unsigned int mask = 0;
	char ch = 0;
	types::keymap<int> keepResult_map;

	TransactionFunctionDescription::MainProcessingStep opstep;

	while ((ch = gotoNextToken( langdescr, si, se)) != 0)
	{
		while (subroutine.callstartar.size() <= subroutine.description.steps.size())
		{
			subroutine.callstartar.push_back( si);
		}
		switch ((MainBlockKeyword)utils::parseNextIdentifier( si, se, g_mainblock_idtab))
		{
			case m_NONE:
				ch = parseNextToken( langdescr, tok, si, se);
				if (ch == ';')
				{
					if (mask)
					{
						subroutine.description.steps.push_back( opstep);
						opstep.clear();
						mask = 0;
					}
				}
				else if (g_optab[ch])
				{
					throw ERROR( si, MSG << "keyword (" << g_mainblock_idtab.tostring() << ") expected instead of operator '" << ch << "'");
				}
				else if (ch == '\'' || ch == '\"')
				{
					throw ERROR( si, MSG << "keyword (" << g_mainblock_idtab.tostring() << ") expected instead of string");
				}
				else if (mask == 0x0)
				{
					throw ERROR( si, MSG << "keyword (" << g_mainblock_idtab.tostring() << ") expected instead of identifier '" << tok << "'");
				}
				else
				{
					throw ERROR( si, MSG << "keyword (FOREACH,INTO,DO) expected instead of '" << tok << "'");
				}
				break;
			case m_ON:
				if (mask != 0) throw std::runtime_error( "database command not terminated with ';'");

				parseErrorHint( subroutine, ERROR, langdescr, si, se);
				break;
			case m_KEEP:
				if (mask != 0) throw std::runtime_error( "database command not terminated with ';'");

				parseKeepAs( keepResult_map, subroutine.description.steps.size(), ERROR, langdescr, si, se);
				break;
			case m_END:
				if (mask != 0) throw std::runtime_error( "database command not terminated with ';'");
				try
				{
					if (subroutine.blockstk.empty())
					{
						if (!subroutine.result_INTO.empty())
						{
							subroutine.description.resultblocks.insert( subroutine.description.resultblocks.begin(), TransactionFunctionDescription::ResultBlock( subroutine.result_INTO, 0, subroutine.description.steps.size()));
						}
					}
					else
					{
						subroutine.blockstk.back().size = subroutine.description.steps.size() - subroutine.blockstk.back().startidx;
						subroutine.description.resultblocks.push_back( subroutine.blockstk.back());
						subroutine.blockstk.pop_back();
					}
				}
				catch (const TransactionFunctionDescription::MainProcessingStep::Error& err)
				{
					throw ERROR( subroutine.callstartar[ err.elemidx], MSG << "error in definition of transaction: " << err.msg);
				}
				catch (const std::runtime_error& err)
				{
					throw ERROR( subroutine.start, MSG << "error in definition of transaction: " << err.what());
				}
				catch (const std::exception& e)
				{
					LOG_ERROR << "uncaught exception loading transaction program: " << e.what();
				}
				catch (...)
				{
					LOG_ERROR << "uncaught exception loading transaction program";
				}
				return;
			case m_PRINT:
				{
					std::vector<std::string> print_INTO_path;
					if (mask != 0)
					{
						unsigned int idshft = (1 << ((unsigned int)m_INTO+1));
						if (mask == idshft)
						{
							print_INTO_path = opstep.path_INTO;
							opstep.path_INTO.clear();
							mask = 0;
						}
						else
						{
							throw std::runtime_error( "unexpected token PRINT in middle of database instruction definition");
						}
					}
					parsePrintStep( subroutine, keepResult_map, ERROR, langdescr, si, se, print_INTO_path);
				}
				break;
			case m_RESULT:
				if (mask != 0) throw std::runtime_error( "database command not terminated with ';'");

				if (0!=(ch=parseNextToken( langdescr, tok, si, se))
				&&  isAlpha(ch) && boost::algorithm::iequals( tok, "INTO"))
				{
					std::vector<std::string> path = parse_INTO_path( langdescr, si, se);
	
					if (0!=(ch=parseNextToken( langdescr, tok, si, se))
					&&  isAlpha(ch) && boost::algorithm::iequals( tok, "BEGIN"))
					{
						subroutine.blockstk.push_back( TransactionFunctionDescription::ResultBlock( path, subroutine.description.steps.size(), 0));
					}
					else
					{
						throw ERROR( si, "keyword (BEGIN) expected after RESULT INTO <path>");
					}
				}
				else
				{
					throw ERROR( si, "keyword (INTO) expected after RESULT");
				}
				break;
			case m_FOREACH:
				checkUniqOccurrence( m_FOREACH, mask, g_mainblock_idtab, ERROR, si);

				ch = utils::parseNextToken( opstep.selector_FOREACH, si, se, utils::emptyCharTable(), utils::anyCharTable());
				if (!ch) throw ERROR( si, "unexpected end of description. sector path expected after FOREACH");
	
				if (isIdentifier(opstep.selector_FOREACH))
				{
					opstep.resultref_FOREACH = getResultNamespaceIdentifier( opstep.selector_FOREACH, keepResult_map, subroutine.description.steps.size());
					if (opstep.resultref_FOREACH >= 0)
					{
						if (opstep.resultref_FOREACH == 0 && boost::algorithm::iequals( opstep.selector_FOREACH, "PARAM"))
						{
							throw ERROR( si, "PARAM not allowed as argument of FOREACH");
						}
						opstep.selector_FOREACH.clear();
					}
				}
				break;
			case m_INTO:
				checkUniqOccurrence( m_INTO, mask, g_mainblock_idtab, ERROR, si);
	
				opstep.path_INTO = parse_INTO_path( langdescr, si, se);
				break;
			case m_DO:
				checkUniqOccurrence( m_DO, mask, g_mainblock_idtab, ERROR, si);
	
				std::string::const_iterator oi = si;
				while (0!=(ch=parseNextToken( langdescr, tok, oi, se)))
				{
					if (isAlpha(ch) && boost::algorithm::iequals( tok, "NONEMPTY"))
					{
						opstep.nonempty = true;
						si = oi;
					}
					else if (isAlpha(ch) && boost::algorithm::iequals( tok, "UNIQUE"))
					{
						opstep.unique = true;
						si = oi;
					}
					else
					{
						break;
					}
				}
				if (!gotoNextToken( langdescr, si, se))
				{
					throw ERROR( si, "unexpected end of transaction description after DO");
				}
				if (langdescr->isEmbeddedStatement( si, se))
				{
					opstep.call = parseEmbeddedStatement( langdescr, si, se, subroutine.param_map, keepResult_map, subroutine.description.steps.size());
				}
				else
				{
					opstep.call = parseCallStatement( langdescr, si, se, subroutine.param_map, keepResult_map, subroutine.description.steps.size());
				}
				break;
		}
	}
}

static std::vector<std::string> parseDatabaseList( config::PositionalErrorMessageBase& ERROR, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	std::vector<std::string> rt;
	std::string dbName;
	for (;;)
	{
		char ch = parseNextToken( langdescr, dbName, si, se);
		if (!ch) throw ERROR( si, "unexpected end of file in DATABASE declaration");
		if (isAlpha(ch))
		{
			rt.push_back( dbName);
			ch = gotoNextToken( langdescr, si, se);
			if (ch == ',')
			{
				++si;
				continue;
			}
			else
			{
				return rt;
			}
		}
		throw ERROR( si, "unexpected token in DATABASE declaration");
	}
}

static bool checkDatabaseList( const std::string& databaseID, const std::string& databaseClassName, config::PositionalErrorMessageBase& ERROR, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	std::vector<std::string> dblist = parseDatabaseList( ERROR, langdescr, si, se);
	std::vector<std::string>::const_iterator di = dblist.begin(), de = dblist.end();
	for (; di != de; ++di)
	{
		if (boost::algorithm::iequals( *di, databaseID)) return true;
		if (boost::algorithm::iequals( *di, databaseClassName)) return true;
	}
	return false;
}

///\brief Forward declaration
static std::vector<std::pair<std::string,TransactionFunctionR> >
	includeFile( const std::string& mainfilename, const std::string& incfilename, const std::string& databaseId, const std::string& databaseClassName, const LanguageDescription* langdescr, SubroutineDeclarationMap& subroutineMap);

static std::string getInclude( const std::string& mainfilename, const std::string& incfilename)
{
	std::string::const_iterator fi = incfilename.begin(), fe = incfilename.end();
	for (; fi != fe; ++fi)
	{
		char ch = *fi | 32;
		if ((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || ch == '_' || ch == '/' || ch == '\\' || ch == '.') continue;
		throw std::runtime_error( "illegal include file: only relative paths allowed for include");
	}
	std::string ext = utils::getFileExtension( incfilename);
	if (ext.empty())
	{
		return utils::readSourceFileContent( utils::joinPath( utils::getParentPath( mainfilename), incfilename) + utils::getFileExtension( mainfilename));
	}
	else
	{
		return utils::readSourceFileContent( utils::joinPath( utils::getParentPath( mainfilename), incfilename));
	}
}

static const char* g_body_ids[] = {"DATABASE","TRANSACTION","SUBROUTINE","RESULT","AUTHORIZE","PREPROC","BEGIN",0};
enum BodyKeyword{ b_NONE, b_DATABASE,b_TRANSACTION,b_SUBROUTINE,b_RESULT,b_AUTHORIZE,b_PREPROC,b_BEGIN};
static const utils::IdentifierTable g_body_idtab( false, g_body_ids);

static bool parseProcedureBody( Subroutine& subroutine, config::PositionalErrorMessageBase& ERROR, const std::string& databaseId, const std::string& databaseClassName, const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se)
{
	unsigned int mask = 0;
	config::PositionalErrorMessageBase::Message MSG;
	char ch;
	std::string tok;

	while (0!=(ch=gotoNextToken( langdescr, si, se)))
	{
		switch ((BodyKeyword)utils::parseNextIdentifier( si, se, g_body_idtab))
		{
			case b_NONE:
				ch = utils::parseNextToken( tok, si, se);
				throw ERROR( si, MSG << "keyword (" << g_body_idtab.tostring() << ") expected instead of identifier '" << tok << "'");
			case b_DATABASE:
				checkUniqOccurrence( b_DATABASE, mask, g_body_idtab, ERROR, si);
				subroutine.isValidDatabase &= checkDatabaseList( databaseId, databaseClassName, ERROR, langdescr, si, se);
				break;
			case b_RESULT:
				checkUniqOccurrence( b_RESULT, mask, g_body_idtab, ERROR, si);
				parseResultDirective( subroutine, ERROR, langdescr, si, se);
				break;
			case b_AUTHORIZE:
				checkUniqOccurrence( b_AUTHORIZE, mask, g_body_idtab, ERROR, si);
				parseAuthorizeDirective( subroutine, ERROR, langdescr, si, se);
				break;
			case b_PREPROC:
				checkUniqOccurrence( b_PREPROC, mask, g_body_idtab, ERROR, si);
				parsePreProcessingBlock( subroutine, ERROR, langdescr, si, se);
				break;
			case b_BEGIN:
				checkUniqOccurrence( b_BEGIN, mask, g_body_idtab, ERROR, si);
				parseMainBlock( subroutine, ERROR, langdescr, si, se);
				if (subroutine.isValidDatabase)
				{
					return true;
				}
				else
				{
					LOG_DEBUG << "parsed but ignored " << (subroutine.isTransaction?"TRANSACTION":"OPERATION") << " '" << subroutine.description.name << "' for active transaction database '" << databaseId << "'";
				}
				return false;
			case b_TRANSACTION:
			case b_SUBROUTINE:
				break;
		}
	}
	if (subroutine.isTransaction)
	{
		throw ERROR( si, "missing main block BEGIN..END in TRANSACTION");
	}
	else
	{
		throw ERROR( si, "missing main block BEGIN..END in SUBROUTINE");
	}
}


static const char* g_toplevel_ids[] = {"DATABASE","TRANSACTION","SUBROUTINE","TEMPLATE","INCLUDE",0};
enum TopLevelKeyword{ t_NONE,t_DATABASE,t_TRANSACTION,t_SUBROUTINE,t_TEMPLATE,t_INCLUDE};
static const utils::IdentifierTable g_toplevel_idtab( false, g_toplevel_ids);

static std::vector<std::pair<std::string,TransactionFunctionR> >
	load( const std::string& filename, const std::string& source, const std::string& databaseId, const std::string& databaseClassName, const LanguageDescription* langdescr, SubroutineDeclarationMap& subroutineMap)
{
	std::vector<std::pair<std::string,TransactionFunctionR> > rt;
	char ch;
	bool firstDef = true;
	bool isValidDatabase = true;
	std::vector<std::string> templateArguments;
	std::string tok;
	std::string::const_iterator si = source.begin(), se = source.end();
	std::string::const_iterator tokstart;

	config::PositionalErrorMessageBase ERROR(source);
	config::PositionalErrorMessageBase::Message MSG;

	if (!langdescr) throw std::logic_error( "no database language description defined");
	try
	{
		while ((ch = gotoNextToken( langdescr, si, se)) != 0)
		{
			tokstart = si;
			bool enterDefinition = false;
			bool isTransaction = false;
			templateArguments.clear();

			switch ((TopLevelKeyword)utils::parseNextIdentifier( si, se, g_toplevel_idtab))
			{
				case t_NONE:
					ch = utils::parseNextToken( tok, si, se);
					throw ERROR( si, MSG << "keyword (" << g_toplevel_idtab.tostring() << ") expected instead of identifier '" << tok << "'");
				case t_DATABASE:
					if (!firstDef)
					{
						throw ERROR( si, MSG << "DATABASE definition only allowed at the beginning of the TDL source (global) or as part of the transaction declaration (per function)");
					}
					isValidDatabase = checkDatabaseList( databaseId, databaseClassName, ERROR, langdescr, si, se);
					if (!isValidDatabase)
					{
						LOG_DEBUG << "TDL file parsed but ignored for database '" << databaseId << "'";
					}
					break;
				case t_TRANSACTION:
					isTransaction = true;
					enterDefinition = true;
					break;
				case t_SUBROUTINE:
					isTransaction = false;
					enterDefinition = true;
					break;
				case t_TEMPLATE:
					if (gotoNextToken( langdescr, si, se) != '<')
					{
						throw ERROR( si, MSG << "template argument list in '<' '>' brackets expected instead of '" << ch << "'");
					}
					templateArguments = parseTemplateArguments( langdescr, si, se);
	
					ch = parseNextToken( langdescr, tok, si, se);
					if (isAlpha(ch) && boost::algorithm::iequals( tok, "SUBROUTINE"))
					{
						isTransaction = false;
						enterDefinition = true;
					}
					else
					{
						throw ERROR( si, MSG << "SUBROUTINE declaration expected after TEMPLATE declaration instead of '" << errorTokenString( ch, tok) << "'");
					}
					break;
				case t_INCLUDE:
					enterDefinition = false;

					ch = utils::parseNextToken( tok, si, se, utils::emptyCharTable(), utils::anyCharTable());
					if (!ch) throw ERROR( si, MSG << "unexpected end of include definition (include file name expected)");
					std::vector<std::pair<std::string,TransactionFunctionR> > lst
						= includeFile( filename, tok, databaseId, databaseClassName, langdescr, subroutineMap);
					rt.insert( rt.begin(), lst.begin(), lst.end());
					break;
			}
			if (enterDefinition)
			{
				std::string::const_iterator dbe = lineStart( tokstart, source);
				std::string transactionName;

				ch = parseNextToken( langdescr, transactionName, si, se);
				if (!ch) throw ERROR( si, MSG << "unexpected end of transaction definition (transaction name expected)");
				if (g_optab[ ch]) throw ERROR( si, MSG << "identifier (transaction name) expected instead of '" << ch << "'");

				Subroutine subroutine( templateArguments, dbe, isTransaction);
				subroutine.description.name = transactionName;
				subroutine.description.casesensitive = langdescr->isCaseSensitive();
				subroutine.isValidDatabase = isValidDatabase;

				if (!subroutine.isTransaction && gotoNextToken( langdescr, si, se) == '(')
				{
					// ... subroutine call argument list
					parseNextToken( langdescr, tok, si, se);
					parseSubroutineArguments( subroutine.param_map, langdescr, si, se);
				}
				try
				{
					if (parseProcedureBody( subroutine, ERROR, databaseId, databaseClassName, langdescr, si, se))
					{
						if (subroutine.isTransaction)
						{
							LOG_TRACE << "Registering transaction definition '" << subroutine.description.name << "'";
							TransactionFunctionR func( createTransactionFunction( subroutine.description, langdescr, subroutineMap));
							rt.push_back( std::pair<std::string,TransactionFunctionR>( subroutine.description.name, func));
						}
						else
						{
							TransactionFunctionR func( createTransactionFunction( subroutine.description, langdescr, subroutineMap));
							subroutineMap[ subroutine.description.name] = SubroutineDeclaration( subroutine.templateArguments, func);
						}
					}
				}
				catch (const TransactionFunctionDescription::MainProcessingStep::Error& err)
				{
					throw ERROR( subroutine.callstartar[ err.elemidx], MSG << "error in definition of transaction: " << err.msg);
				}
				catch (const TransactionFunctionDescription::PreProcessingStep::Error& err)
				{
					throw ERROR( subroutine.pprcstartar[ err.elemidx], MSG << "error in definition of transaction: " << err.msg);
				}

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
	return rt;
}

static std::vector<std::pair<std::string,TransactionFunctionR> >
	includeFile( const std::string& mainfilename, const std::string& incfilename, const std::string& databaseId, const std::string& databaseClassName, const LanguageDescription* langdescr, SubroutineDeclarationMap& subroutineMap)
{
	try
	{
		std::string incsrc = getInclude( mainfilename, incfilename);
		return load( mainfilename, incsrc, databaseId, databaseClassName, langdescr, subroutineMap);
	}
	catch (const config::PositionalErrorException& e)
	{
		config::PositionalFileError err( e, incfilename);
		throw config::PositionalFileErrorException( err);
	}
	catch (const std::runtime_error& e)
	{
		LOG_ERROR << "error loading program from file '" << mainfilename << " when including file '" << incfilename << "'";
		throw e;
	}
	catch (...)
	{
		LOG_ERROR << "uncaught exception in TDL include file '" << incfilename << "' included from file '" << mainfilename << "'";
		throw std::runtime_error( "uncaught exception. see logs");
	}
}

std::vector<std::pair<std::string,TransactionFunctionR> >
	_Wolframe::db::loadTransactionProgramFile(
		const std::string& filename,
		const std::string& databaseId,
		const std::string& databaseClassName,
		const LanguageDescription* langdescr)
{
	try
	{
		SubroutineDeclarationMap subroutineMap;
		return load( filename, utils::readSourceFileContent( filename), databaseId, databaseClassName, langdescr, subroutineMap);
	}
	catch (const config::PositionalFileErrorException& e)
	{
		throw e;
	}
	catch (const config::PositionalErrorException& e)
	{
		config::PositionalFileError err( e, filename);
		throw config::PositionalFileErrorException( err);
	}
	catch (const std::runtime_error& e)
	{
		LOG_ERROR << "error loading program from file '" << filename << "'";
		throw e;
	}
	catch (...)
	{
		LOG_ERROR << "uncaught exception loading program from file '" << filename << "'";
		throw std::runtime_error( "uncaught exception. see logs");
	}
}


