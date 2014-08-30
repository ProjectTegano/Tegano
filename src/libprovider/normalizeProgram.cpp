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
//\brief Implementation of loading programs for token normalization
//\file prgbind_normalizeProgram.cpp
#include "normalizeProgram.hpp"
#include "types/customDataNormalizer.hpp"
#include "types/normalizeFunction.hpp"
#include "types/variant.hpp"
#include "types/customDataType.hpp"
#include "utils/parseUtils.hpp"
#include "utils/fileUtils.hpp"
#include "utils/fileLineInfo.hpp"
#include "processor/procProviderInterface.hpp"
#include "logger/logger-v1.hpp"
#include <string>

using namespace _Wolframe;
using namespace _Wolframe::prgbind;

static types::NormalizeFunctionR createBaseFunction( const std::string& name, const std::vector<types::Variant>& arg, const ProgramLibrary& prglibrary)
{
	try
	{
		const types::NormalizeFunctionType* normalizer = prglibrary.getNormalizeFunctionType( name);
		const types::CustomDataType* customtype = prglibrary.getCustomDataType( name);
		if (normalizer)
		{
			if (customtype) throw std::runtime_error( std::string( "amgibuus definition of custom data type and normalize function '") + name + "'");
			return types::NormalizeFunctionR( normalizer->createFunction( arg));
		}
		else if (customtype)
		{
			return types::NormalizeFunctionR( new types::CustomDataNormalizer( name, arg, customtype));
		}
		else
		{
			throw std::runtime_error( std::string("no normalize function or custom data type defined for '") + name + "'");
		}
	}
	catch (const std::bad_alloc& err)
	{
		throw err;
	}
	catch (const std::runtime_error& err)
	{
		std::string argstr;
		std::vector<types::Variant>::const_iterator ai = arg.begin(), ae = arg.end();
		for (int cnt=0; ai != ae; ++ai,cnt++)
		{
			if (cnt) argstr.push_back( ',');
			argstr.push_back( '"');
			argstr.append( ai->tostring());
			argstr.push_back( '"');
		}
		throw std::runtime_error( std::string("could not build normalizer for '") + name + "(" + argstr + ")' :" + err.what());
	}
}

class CombinedNormalizeFunction :public types::NormalizeFunction
{
public:
	CombinedNormalizeFunction( const std::string& name_)
		:m_name(name_){}
	CombinedNormalizeFunction( const CombinedNormalizeFunction& o)
		:types::NormalizeFunction(), m_name(o.m_name), m_steps(o.m_steps){}

	void define( const types::NormalizeFunctionR& f)
	{
		m_steps.push_back(f);
	}

	bool hasMethod( const std::string& methodname)
	{
		if (m_steps.empty()) return false;
		const types::NormalizeFunction* nf = m_steps.back().get();
		const types::CustomDataNormalizer* cf = dynamic_cast<const types::CustomDataNormalizer*>( nf);
		if (!cf) return false;
		const types::CustomDataType* tp = cf->type();
		if (!tp) return false;
		return tp->getMethod( methodname);
	}

	virtual const char* name() const
	{
		return m_name.c_str();
	}

	virtual types::Variant execute( const types::Variant& i) const
	{
		if (m_steps.empty()) return i;
		std::vector<types::NormalizeFunctionR>::const_iterator fi = m_steps.begin(), fe = m_steps.end();
		types::Variant rt = (*fi++)->execute( i);
		for (; fi != fe; ++fi)
		{
			rt = (*fi)->execute( rt);
		}
		return rt;
	}

	virtual types::NormalizeFunction* copy() const
	{
		return new CombinedNormalizeFunction( *this);
	}

	std::size_t nofSteps() const
	{
		return m_steps.size();
	}

	const types::NormalizeFunctionR& operator[]( std::size_t i) const
	{
		return m_steps[i];
	}

private:
	std::string m_name;
	std::vector<types::NormalizeFunctionR> m_steps;
};


static std::vector<std::pair<std::string,types::NormalizeFunctionR> >
	loadSource( const std::string& filename, const std::string& source, const ProgramLibrary& prglibrary)
{
	std::vector<std::pair<std::string,types::NormalizeFunctionR> > rt;
	static const utils::CharTable optab( "#=;)(,");
	std::string prgname,tok,funcname;
	std::string::const_iterator argstart;
	std::string::const_iterator si = source.begin(), se = source.end();

	utils::FileLineInfo posinfo( utils::getFileStem( filename) + utils::getFileExtension( filename));
	std::string::const_iterator posinfo_si = si;
	
	char ch;

	try
	{
		while ((ch = utils::gotoNextToken( si, se)) != 0)
		{
			switch ((ch=utils::parseNextToken( prgname, si, se, optab)))
			{
				case '#': utils::parseLine( si, se); continue; //... comment
				case ';': throw std::runtime_error( "empty statement");
				case '\'':
				case '\"': throw std::runtime_error( "identifier expected instead of string at start of statement");
				default:
					if (optab[ ch]) throw std::runtime_error( "identifier expected at start of statement");
			}
			CombinedNormalizeFunction funcdef( prgname);

			switch ((ch=utils::parseNextToken( tok, si, se, optab)))
			{
				case '\0': throw std::runtime_error( "unexpected end of program");
				case '=': break;
				default:
					if (optab[ ch]) throw std::runtime_error( std::string("'=' expected instead of '") + ch + "'");
					throw std::runtime_error( std::string("'=' expected instead of '") + tok + "'");
			}
			while (ch != ';')
			{
				switch ((ch=utils::parseNextToken( funcname, si, se, optab)))
				{
					case '\0': throw std::runtime_error( "unexpected end of program");
					default:
						if (optab[ ch]) throw std::runtime_error( std::string( "function name identifier expected instead of '") + ch + "'");
				}
				switch ((ch=utils::gotoNextToken( si, se)))
				{
					case '\0': throw std::runtime_error( "unexpected end of program");
					case ',':
					case ';':
						if (funcdef.hasMethod( funcname))
						{
							funcdef.define( types::NormalizeFunctionR( new types::CustomDataMethodCallNormalizer( funcname, std::vector<types::Variant>())));
						}
						else
						{
							funcdef.define( types::NormalizeFunctionR( createBaseFunction( funcname, std::vector<types::Variant>(), prglibrary)));
						}
						++si;
						continue;
					case '(':
					{
						++si;
						std::vector<types::Variant> arg;
						do
						{
							ch = utils::parseNextToken( tok, si, se, optab);
							if (ch == '\0') throw std::runtime_error( "unexpected end of program");
							if (ch == '(') throw std::runtime_error( "nested expressions, bracket not closed");
							if (ch == ')')
							{
								if (arg.empty()) break;
								throw std::runtime_error( "unexpected token ')', argument expected");
							}
							if (ch == '=') throw std::runtime_error( "unexpected token '='");
							if (ch == ';') throw std::runtime_error( "unexpected end of expression, bracket not closed");
							if (ch == ',') throw std::runtime_error( "unexpected token ',', argument expected");
							arg.push_back( types::Variant( tok));
						}
						while ((ch=utils::parseNextToken( tok, si, se, optab)) == ',');
						if (ch != ')') throw std::runtime_error( "expected ')' or argument separator ','");
						
						if (funcdef.hasMethod( funcname))
						{
							funcdef.define( types::NormalizeFunctionR( new types::CustomDataMethodCallNormalizer( funcname, arg)));
						}
						else
						{
							funcdef.define( types::NormalizeFunctionR( createBaseFunction( funcname, arg, prglibrary)));
						}
						ch = utils::gotoNextToken( si, se);
						if (ch == ';' || ch == ',')
						{
							++si;
							continue;
						}
						if (!ch) throw std::runtime_error( "unexpected end of program");
						throw std::runtime_error( "unexpected token at end of expression");
					}
					default:
						throw std::runtime_error( std::string( "separator ',' or ';' expected or function arguments in '(' ')' brackets instead of '") + ch + "'");
				}
			}
			types::NormalizeFunctionR func( new CombinedNormalizeFunction( funcdef));
			rt.push_back( std::pair<std::string,types::NormalizeFunctionR>( prgname, func));
		}
		return rt;
	}
	catch (const std::runtime_error& e)
	{
		posinfo.update( posinfo_si, si);
		throw std::runtime_error( std::string(e.what()) + " " + posinfo.logtext());
	}
}

bool NormalizeProgram::is_mine( const std::string& filename) const
{
	std::string ext = utils::getFileExtension( filename);
	if (boost::algorithm::iequals( ext, ".normalize"))
	{
		LOG_WARNING << "Using deprecated file extension for program '.normalize' instead of '.wnmp'";
		return true;
	}
	if (boost::algorithm::iequals( ext, ".wnmp")) return true;
	return false;
}

void NormalizeProgram::loadProgram( ProgramLibrary& library, db::Database*, const std::string& filename)
{
	try
	{
		std::vector<std::pair<std::string,types::NormalizeFunctionR> > funclist
			= loadSource( filename, utils::readSourceFileContent( filename), library);

		std::vector<std::pair<std::string,types::NormalizeFunctionR> >::const_iterator ni = funclist.begin(), ne = funclist.end();
		for (; ni != ne; ++ni)
		{
			library.defineNormalizeFunction( ni->first, ni->second);
		}
	}
	catch (const std::runtime_error& err)
	{
		throw std::runtime_error( std::string( "error in normalize program: ") + err.what());
	}
}



