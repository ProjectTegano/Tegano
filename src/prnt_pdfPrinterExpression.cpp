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
///\file prnt_pdfPrinterExpression.cpp
#include "prnt/pdfPrinterExpression.hpp"
#include "prnt/pdfPrinterVariable.hpp"
#include "types/bcdArithmetic.hpp"
#include "utils/miscUtils.hpp"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <limits>

using namespace std;

using namespace _Wolframe;
using namespace _Wolframe::prnt;

void Expression::push_operator( char chr)
{
	Item ee;
	ee.m_type = Item::Operator;
	ee.value.m_opchr = chr;
	m_ar.push_back( ee);
}

void Expression::push_value( std::size_t idx)
{
	Item ee;
	ee.m_type = Item::Value;
	ee.value.m_idx = idx;
	m_ar.push_back( ee);
}

void Expression::push_variable( Variable::Id var)
{
	Item ee;
	ee.m_type = Item::Variable;
	ee.value.m_idx = (std::size_t)var;
	m_ar.push_back( ee);
}

void Expression::push_tagvariable( Variable::Id var)
{
	Item ee;
	ee.m_type = Item::TagVariable;
	ee.value.m_idx = (std::size_t)var;
	m_ar.push_back( ee);
}

void Expression::push_expression( const Expression& expr)
{
	m_ar.insert( m_ar.end(), expr.m_ar.begin(), expr.m_ar.end());
}

typedef types::BigNumber Number;

void Expression::evaluate( VariableScope& vscope, const std::string& exprstrings) const
{
	ValueStack stk;
	std::vector<Item>::const_iterator next,itr=m_ar.begin(), end=m_ar.end();
	for (; itr != end; ++itr)
	{
		bool passToSibling = false;
		switch (itr->m_type)
		{
			case Item::TagVariable:
				passToSibling = true;
				/*no break here*/
			case Item::Variable:
				next = itr+1;
				if (next != end && next->m_type == Item::Operator && (next->value.m_opchr == '=' || next->value.m_opchr == '?'))
				{
					// handle special case of variable on left side of assignment:
					Variable::Id var = (Variable::Id)itr->value.m_idx;
					if (next->value.m_opchr == '?')
					{
						// conditional assigment (define if not yet defined)
						if (!vscope.isDefined( var))
						{
							vscope.define( var, stk.top(), passToSibling);
						}
						else if (passToSibling)
						{
							vscope.define( var, var, passToSibling);
						}
						stk.pop();
					}
					else
					{
						// normal assigment
						vscope.define( var, stk.top(), passToSibling);
						stk.pop();
					}
					++itr;
				}
				else
				{
					// for other variables are the referenced values expanded:
					Variable::Id var = (Variable::Id)itr->value.m_idx;
					std::size_t idx = vscope.getValueIdx( var);

					if (idx == 0) throw std::runtime_error( std::string( "variable not defined '") + variableName(var) + "'");
					stk.push( vscope.getValue( idx));
				}
				break;

			case Item::Value:
				stk.push( std::string( exprstrings.c_str() + itr->value.m_idx));
				break;

			case Item::Operator:
				switch (itr->value.m_opchr)
				{
					case '=':
						throw std::logic_error("internal: assignement '=' with illegal operands");
					break;
					case '?':
						throw std::logic_error("internal: conditional assignement '?=' with illegal operands");
					break;
					case '~':
					{
						Number op( stk.top());
						stk.pop();
						stk.push( (-op).tostring());
						break;
					}
					case '*':
					{
						Number op1( stk.top(0), m_calc_precision);
						Number op2( stk.top(1), m_calc_precision);
						stk.pop( 2);
						stk.push( Number( op1 * op2).tostring());
						break;
					}
					case '/':
					{
						Number op1( stk.top(0), m_calc_precision);
						Number op2( stk.top(1), m_calc_precision);
						stk.pop( 2);
						stk.push( Number( op1 / op2).tostring());
						break;
					}
					case '+':
					{
						Number op1( stk.top(0), m_calc_precision);
						Number op2( stk.top(1), m_calc_precision);
						stk.pop( 2);
						stk.push( Number( op1 + op2).tostring());
						break;
					}
					case '-':
					{
						Number op1( stk.top(0), m_calc_precision);
						Number op2( stk.top(1), m_calc_precision);
						stk.pop( 2);
						stk.push( Number( op1 - op2).tostring());
						break;
					}
					case '#':
					{
						std::string op1( stk.top(0));
						std::string op2( stk.top(1));
						stk.pop( 2);
						stk.push( op1 + op2);
						break;
					}
					default:
						throw std::logic_error("internal: unknown operator in expression");
				}
			break;
		}
	}
}

static bool checkNumber( const std::string& tok)
{
	int state = 0;
	std::string::const_iterator ii = tok.begin(), ee = tok.end();
	for (; ii != ee; ++ii)
	{
		if (*ii <= '9' && *ii >= '0') continue;
		if (*ii == '.')
		{
			if (state != 0) return false;
			state = 1;
			continue;
		}
		return false;
	}
	return true;
}

static bool isNumberToken( char ch)
{
	return (ch >= '0' && ch <= '9');
}

static bool checkIdentifier( const std::string& tok)
{
	std::string::const_iterator ii = tok.begin(), ee = tok.end();
	for (; ii != ee; ++ii)
	{
		if ((*ii|32) <= 'z' && (*ii|32) >= 'a') continue;
		if (*ii == '_') continue;
		if (*ii <= '9' && *ii >= '0' && ii != tok.begin()) continue;
		return false;
	}
	return true;
}

static bool isIdentifierToken( char ch)
{
	return (((ch|32) >= 'a' && (ch|32) <= 'z') || ch == '_');
}

static bool isSpace( char ch)
{
	return ((unsigned char)ch <= (unsigned char)' ');
}

static void skipSpaces( std::string::const_iterator itr, const std::string::const_iterator& end)
{
	while (itr != end && isSpace(*itr)) ++itr;
}

static Expression parseOperand( std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings);
static Expression parseSumExpression( const Expression& op1, std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings);
static Expression parseFactorExpression( const Expression& op1, std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings);
static Expression parseAssignExpression( const Expression& op1, std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings);
static Expression parseAssignExpressionList( char separator, std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings);
static StateDef::MethodCall parseMethodCall( std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings);
static utils::CharTable g_operatorTable( "#(){}[]=+-*/,;?");


static Expression parseOperand( std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings)
{
	Expression rt;
	std::string tok;
	std::string::const_iterator prev;
	char ch;

	prev = itr; ch = parseNextToken( tok, itr, end, g_operatorTable);
	switch (ch)
	{
		case '\0': break;
		case ',':
		case ';':
		case '}':
		case ')': itr = prev; break;
		case '-':
		{
			Expression subexpr = parseOperand( itr, end, exprstrings);
			if (!subexpr.size()) throw std::runtime_error( "unary operator without argument in expression");
			rt.push_expression( subexpr);
			rt.push_operator( '~');
			break;
		}
		case '+':
		{
			Expression subexpr = parseOperand( itr, end, exprstrings);
			if (!subexpr.size()) throw std::runtime_error( "unary operator without argument in expression");
			rt.push_expression( subexpr);
			break;
		}
		case '(':
		{
			Expression op1 = parseOperand( itr, end, exprstrings);
			if (!op1.size()) throw std::runtime_error( "empty subexpression");
			Expression subexpr = parseSumExpression( op1, itr, end, exprstrings);
			prev = itr; ch = parseNextToken( tok, itr, end, g_operatorTable);
			if (ch != ')') throw std::runtime_error( "subexpression not terminated. ')' expected");
			rt.push_expression( subexpr);
			break;
		}
		case '#':
		case '=':
		case '/':
		case '*':
			throw std::runtime_error( "identifier string or number expected");
		case '\'':
		case '"':
			rt.push_value( exprstrings.size());
			exprstrings.append( tok);
			exprstrings.push_back( '\0');
			break;
		default:
		{
			if (isIdentifierToken(ch))
			{
				if (!checkIdentifier( tok)) throw std::runtime_error( std::string( "illegal identifier '") + tok + "'");
				rt.push_variable( variableId( tok));
			}
			else if (isNumberToken(ch))
			{
				if (!checkNumber( tok)) throw std::runtime_error( std::string( "illegal number '") + tok + "'");
				rt.push_value( exprstrings.size());
				exprstrings.append( tok);
				exprstrings.push_back( '\0');
			}
			else
			{
				throw std::runtime_error( std::string( "unexpected token '") + tok + "'");
			}
		}
	}
	return rt;
}

static Expression parseFactorExpression( const Expression& op1, std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings)
{
	Expression rt;
	std::string tok;
	std::string::const_iterator prev;
	char ch;

	prev = itr; ch = parseNextToken( tok, itr, end, g_operatorTable);
	switch (ch)
	{
		case '\0':
			break;
		case ',':
		case ';':
		case ')':
		case '}':
		case '+':
		case '#':
		case '-':
			itr = prev;
			rt.push_expression( op1);
			break;
		case '/':
		case '*':
		{
			Expression op2 = parseOperand( itr, end, exprstrings);
			if (!op2.size()) throw std::runtime_error( "second operand expected for binary minus '-'");
			rt.push_expression( op1);
			rt.push_expression( op2);
			rt.push_operator( ch);
			break;
		}
		default:
		{
			throw std::runtime_error( "binary operator {'+','-','/','*'} expected");
		}
	}
	return rt;
}

static Expression parseSumExpression( const Expression& op1, std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings)
{
	Expression rt;
	std::string tok;
	std::string::const_iterator prev;
	char ch;

	prev = itr; ch = parseNextToken( tok, itr, end, g_operatorTable);
	switch (ch)
	{
		case '\0':
			break;
		case ';':
		case ',':
		case ')':
		case '}':
			itr = prev;
			rt.push_expression( op1);
			break;
		case '-':
		case '+':
		case '#':
		{
			Expression op2 = parseOperand( itr, end, exprstrings);
			if (!op2.size()) throw std::runtime_error( "second operand expected for binary minus '-'");
			rt.push_expression( op1);
			rt.push_expression( op2);
			rt.push_operator( ch);
			break;
		}
		case '*':
		case '/':
		{
			Expression op2 = parseFactorExpression( op1, itr, end, exprstrings);
			if (!op2.size()) throw std::runtime_error( "second operand expected for binary minus '-'");
			rt.push_expression( op1);
			rt.push_expression( op2);
			rt.push_operator( ch);
			break;
		}
		default:
		{
			throw std::runtime_error( "binary operator {'+','-','/','*'} expected");
		}
	}
	return rt;
}

static Expression parseAssignExpression( const Expression& op1, std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings)
{
	Expression rt;
	std::string tok;
	std::string::const_iterator prev;
	char ch;

	prev = itr; ch = parseNextToken( tok, itr, end, g_operatorTable);
	if (ch == '?')
	{
		if (itr != end && *itr == '=')
		{
			ch = '?';
			++itr;
		}
	}
	switch (ch)
	{
		case '?':
		case '=':
		{
			Expression op2 = parseOperand( itr, end, exprstrings);
			if (!op2.size()) throw std::runtime_error( "something expected right of an assignment '='");
			op2 = parseSumExpression( op2, itr, end, exprstrings);
			if (!op2.size()) throw std::runtime_error( "expression expected right of an assignment '='");
			rt.push_expression( op2);
			rt.push_expression( op1);
			rt.push_operator( ch);
			break;
		}
		default:
		{
			throw std::runtime_error( "binary assignment operator '=' or '?=' expected");
		}
	}
	return rt;
}

static Expression parseAssignExpressionList( char separator, std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings)
{
	Expression rt;
	std::string tok;
	std::string::const_iterator prev;
	char ch;

	prev = itr; ch = parseNextToken( tok, itr, end, g_operatorTable);
	switch (ch)
	{
		case ')':
		case '}':
			itr = prev;
		case '\0':
			return rt;
	}
	for (;;)
	{
		bool passToSibling = false;
		if (ch == '[')
		{
			if (separator == ',')
			{
				throw std::runtime_error( "unexpected token '['. shared variable definition not allowed in parameter list");
			}
			ch = parseNextToken( tok, itr, end, g_operatorTable);
			if (!isIdentifierToken(ch)) throw std::runtime_error( std::string( "variable name expected after '[' instead of '") + tok + "'");
			skipSpaces( itr, end);
			if (itr != end && *itr == ']')
			{
				++itr;
				passToSibling = true;
			}
			else
			{
				throw std::runtime_error( std::string( "']' expected after variable definition for shared variable in tag context [") + tok + "]");
			}
		}
		if (isIdentifierToken(ch))
		{
			Expression op1;
			if (!checkIdentifier( tok)) throw std::runtime_error( std::string( "illegal identifier '") + tok + "'");
			if (passToSibling)
			{
				op1.push_tagvariable( variableId( tok));
			}
			else
			{
				op1.push_variable( variableId( tok));
			}
			rt.push_expression( parseAssignExpression( op1, itr, end, exprstrings));
			prev = itr; ch = parseNextToken( tok, itr, end, g_operatorTable);
			if (ch == separator)
			{
				ch = parseNextToken( tok, itr, end, g_operatorTable);
				continue;
			}
			switch (ch)
			{
				case ')':
				case '}':
					itr = prev;
				case '\0':
					return rt;
			}
			throw std::runtime_error( std::string( "separator or end of assignment list expected; unexpected token '") + tok + "'");
		}
		else
		{
			throw std::runtime_error( std::string( "identifier or identifier in [ ] brackets expected; unexpected token '") + tok + "'");
		}
	}
}

static StateDef::MethodCall parseMethodCall( std::string::const_iterator& itr, const std::string::const_iterator& end, std::string& exprstrings)
{
	StateDef::MethodCall rt;
	std::string tok;
	std::string::const_iterator prev;
	char ch;

	ch = parseNextToken( tok, itr, end, g_operatorTable);
	if (!isIdentifierToken(ch)) throw std::runtime_error( std::string( "expected method identifier instead of ' ") + tok + "'");

	rt.m_method = methodId( tok);
	ch = parseNextToken( tok, itr, end, g_operatorTable);
	if (ch == '(')
	{
		rt.m_param.push_expression( parseAssignExpressionList( ',', itr, end, exprstrings));
		ch = parseNextToken( tok, itr, end, g_operatorTable);
		if (ch != ')') throw std::runtime_error( "expected ')' at end of method parameter list");
		skipSpaces( itr, end);
		if (itr == end)
		{}
		else if (*itr == ';')
		{
			++itr;
		}
		else
		{
			throw std::runtime_error( "';' or end of line expected after method call");
		}
	}
	else
	{
		if (ch != ';' && ch != '\0') throw std::runtime_error( std::string( "';' or end of line expected instead of '") + tok + "'");
	}
	return rt;
}

void StateDef::parse( std::string::const_iterator itr, const std::string::const_iterator& end, std::string& exprstrings)
{
	std::string tok;
	std::string::const_iterator prev;
	Expression expr;
	char ch;

	skipSpaces( itr, end);
	if (itr != end)
	{
		if (*itr == '{')
		{
			++itr;
			m_expr.push_expression( parseAssignExpressionList( ';', itr, end, exprstrings));
			ch = parseNextToken( tok, itr, end, g_operatorTable);
			if (ch != '}') throw std::runtime_error( "expected '}' at end of assignment list");
		}
		skipSpaces( itr, end);
	}
	while (itr != end)
	{
		m_call.push_back( parseMethodCall( itr, end, exprstrings));
		skipSpaces( itr, end);
	}
}

std::string Expression::tostring( const std::string& exprstrings) const
{
	std::ostringstream out;
	Variable::Id var;
	std::vector<Item>::const_iterator itr=m_ar.begin(), end=m_ar.end();
	for (; itr != end; ++itr)
	{
		switch (itr->m_type)
		{
			case Item::TagVariable:
				var = (Variable::Id)itr->value.m_idx;
				out << " [" << variableName( var) << "]";
				break;

			case Item::Variable:
				var = (Variable::Id)itr->value.m_idx;
				out << " " << variableName( var);
				break;

			case Item::Value:
				out << " '" << std::string( exprstrings.c_str() + itr->value.m_idx) << "'";
				break;

			case Item::Operator:
				out << " " << itr->value.m_opchr;
				break;
			break;
		}
	}
	return out.str();
}

std::string StateDef::MethodCall::tostring( const std::string& exprstrings) const
{
	std::ostringstream out;
	out << methodName( m_method) << "(" << m_param.tostring( exprstrings) << " )";
	return out.str();
}

std::string StateDef::tostring( const std::string& exprstrings) const
{
	std::ostringstream out;
	std::vector<MethodCall>::const_iterator ii = m_call.begin(), ee = m_call.end();
	out << "{" << m_expr.tostring( exprstrings) << " }";
	for (; ii != ee; ++ii)
	{
		out << " " << ii->tostring( exprstrings) << ";";
	}
	return out.str();
}

