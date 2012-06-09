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
///\file langbind_appObjects.cpp
///\brief implementation of scripting language objects
#include "langbind/appObjects.hpp"
#include "serialize/ddl/filtermapDDLParse.hpp"
#include "serialize/ddl/filtermapDDLPrint.hpp"
#include "ddl/compiler/simpleFormCompiler.hpp"
#include "logger-v1.hpp"
#include "filter/filter.hpp"
#include "filter/char_filter.hpp"
#include "filter/line_filter.hpp"
#include "filter/token_filter.hpp"
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>

#ifdef WITH_LIBXML2
#include "filter/libxml2_filter.hpp"
#endif
#include "filter/textwolf_filter.hpp"
#ifdef WITH_XMLLITE
#include "filter/xmllite_filter.hpp"
#endif
#if WITH_MSXML
#include "filter/msxml_filter.hpp"
#endif

using namespace _Wolframe;
using namespace langbind;

namespace //anonymous
{
template <class Object>
void defineObject( std::map<std::string,Object>& m_map, const std::string& name, const Object& obj)
{
	std::string nam( name);
	std::transform( nam.begin(), nam.end(), nam.begin(), ::tolower);
	m_map[ nam] = obj;
}

template <class Object>
bool getObject( const std::map<std::string,Object>& m_map, const std::string& name, Object& obj)
{
	std::string nam( name);
	std::transform( nam.begin(), nam.end(), nam.begin(), ::tolower);
	typename std::map<std::string,Object>::const_iterator ii=m_map.find( nam),ee=m_map.end();
	if (ii == ee)
	{
		return false;
	}
	else
	{
		obj = ii->second;
		return true;
	}
}
}//anonymous namespace

void FilterMap::defineFilter( const std::string& name, const FilterFactoryR& f)
{
	defineObject( m_map, name, f);
}

bool FilterMap::getFilter( const std::string& arg, Filter& rt)
{
	std::size_t nn = arg.size();
	std::size_t ii = nn;
	std::size_t aa = nn;
	std::string nam( arg);
	std::transform( nam.begin(), nam.end(), nam.begin(), ::tolower);
	do
	{
		nam.resize(ii);
		std::map<std::string,FilterFactoryR>::const_iterator itr=m_map.find( nam),end=m_map.end();
		if (itr != end)
		{
			rt = itr->second->create( (ii==nn)?0:(arg.c_str()+aa));
			return true;
		}
		for (nn=ii; ii>0 && arg[ii] != ':'; --ii);
		aa = ii + 1;
		for (++ii; ii>0 && arg[ii-1] == ':'; --ii);
	}
	while (ii>0 && ii<nn);
	return false;
}

FilterMap::FilterMap()
{
	defineFilter( "char", FilterFactoryR( new CharFilterFactory()));
	defineFilter( "line", FilterFactoryR( new LineFilterFactory()));
	defineFilter( "xml:textwolf", FilterFactoryR( new TextwolfXmlFilterFactory()));
#ifdef WITH_LIBXML2
	defineFilter( "xml:libxml2", FilterFactoryR( new Libxml2FilterFactory()));
#endif
#ifdef WITH_XMLLITE
	defineFilter( "xml:xmllite", FilterFactoryR( new Libxml2FilterFactory()));
#endif
#ifdef WITH_MSXML
	defineFilter( "xml:msxml", FilterFactoryR( new Libxml2FilterFactory()));
#endif
}

void DDLFormMap::defineForm( const std::string& name, const DDLForm& f)
{
	defineObject( m_map, name, f);
}

bool DDLFormMap::getForm( const std::string& name, DDLFormR& rt) const
{
	rt = DDLFormR( new DDLForm());
	return getObject( m_map, name, *rt.get());
}

FormFunction::CallResult FormFunction::call( InputFilter& ifl, OutputFilter& ofl, CallContext& ctx)
{
	if (ctx.m_lastres == Error) return Error;
	void* param_struct = ctx.m_data.get();
	void* result_struct = (void*)((char*)ctx.m_data.get() + ctx.m_api_param->size());

	switch (ctx.m_state)
	{
		case 0:
			ctx.m_ctx.clear();
			if (!ctx.m_api_param->init( param_struct))
			{
				ctx.m_ctx.setError( "could not initialize api input object for form function");
				return ctx.m_lastres=Error;
			}
			ctx.m_state = 1;
		case 1:
			if (!ctx.m_api_result->init( result_struct))
			{
				ctx.m_ctx.setError( "could not initialize api result object for form function");
				return ctx.m_lastres=Error;
			}
			ctx.m_state = 2;
		case 2:
			if (!ctx.m_api_param->parse( param_struct, ifl, ctx.m_ctx, ctx.m_parsestk))
			{
				switch (ifl.state())
				{
					case InputFilter::Open:
						if (ctx.m_ctx.getLastError())
						{
							return ctx.m_lastres=Error;
						}
						break;

					case InputFilter::EndOfMessage:
						return ctx.m_lastres=Yield;

					case InputFilter::Error:
						ctx.m_ctx.setError( ifl.getError());
						return ctx.m_lastres=Error;
				}
			}
			ctx.m_state = 3;
		case 3:
			try
			{
				ctx.m_ctx.clear();
				int rt = m_call( result_struct, param_struct);
				if (rt != 0)
				{
					ctx.m_ctx.setError( "error in call of form function");
					return ctx.m_lastres=Error;
				}
			}
			catch (const std::exception& e)
			{
				ctx.m_ctx.setError( e.what());
				return ctx.m_lastres=Error;
			}
			ctx.m_state = 4;
		case 4:
			if (!ctx.m_api_param->print( result_struct, ofl, ctx.m_ctx, ctx.m_printstk))
			{
				switch (ofl.state())
				{
					case OutputFilter::Open:
						return ctx.m_lastres=Error;

					case OutputFilter::EndOfBuffer:
						return ctx.m_lastres=Yield;

					case OutputFilter::Error:
						ctx.m_ctx.setError( ofl.getError());
						return ctx.m_lastres=Error;
				}
			}
			ctx.m_state = 5;
		case 5:
			ctx.m_api_param->done( param_struct);
			ctx.m_api_result->done( result_struct);
			std::memset( ctx.m_data.get(), 0, ctx.m_api_param->size() + ctx.m_api_result->size());
			ctx.m_state = 0;
	}
	return ctx.m_lastres=Ok;
}

FormFunction::CallContext::~CallContext()
{
	if (m_state >= 0)
	{
		void* param_struct = m_data.get();
		void* result_struct = (void*)((char*)m_data.get() + m_api_param->size());

		switch (m_state)
		{
			case 0:
				break;
			case 5:
			case 4:
			case 3:
			case 2:
				m_api_result->done( result_struct);
			case 1:
				m_api_param->done( param_struct);
		}
	}
}

void FormFunctionMap::defineFormFunction( const std::string& name, const FormFunction& f)
{
	defineObject( m_map, name, f);
}

bool FormFunctionMap::getFormFunction( const std::string& name, FormFunction& rt) const
{
	return getObject( m_map, name, rt);
}

bool TransactionFunction::call( const DDLForm& param, DDLForm& result)
{
	cmdbind::CommandHandler* cmd = m_cmd.get();
	if (!cmd || !m_cmdwriter.get() || !m_resultreader.get())
	{
		LOG_ERROR << "incomplete transaction function call definition";
		return false;
	}
	enum {inbufsize=4096, outbufsize=4096};
	char inbuf[ inbufsize];
	char outbuf[ outbufsize];

	cmd->setInputBuffer( inbuf, inbufsize);
	cmd->setOutputBuffer( outbuf, outbufsize, 0);
	m_cmdwriter->setOutputBuffer( inbuf, inbufsize);

	std::string resultstr;
	cmdbind::CommandHandler::Operation op = cmdbind::CommandHandler::READ;

	for(;;)
	{
		switch (op)
		{
			case cmdbind::CommandHandler::READ:
			{
				if (!serialize::print( param.m_struct, *m_cmdwriter, m_ctx, m_printstk))
				{
					const char* err = m_ctx.getLastError();
					if (err)
					{
						LOG_ERROR << "error in transaction function command write (" << err << ")";
						return false;
					}
				}
				cmd->putInput( inbuf, m_cmdwriter->getPosition());
				op = cmd->nextOperation();
				continue;
			}
			case cmdbind::CommandHandler::WRITE:
			{
				const void* data;
				std::size_t datasize;
				cmd->getOutput( data, datasize);

				m_resultreader->putInput( data, datasize, false);
				if (!serialize::parse( result.m_struct, *m_resultreader, m_ctx, m_parsestk))
				{
					const char* err = m_ctx.getLastError();
					if (err)
					{
						LOG_ERROR << "error in transaction function result read (" << err << ")";
						return false;
					}
				}
				op = cmd->nextOperation();
				continue;
			}
			case cmdbind::CommandHandler::CLOSED:
			{
				m_resultreader->putInput( "", 0, true);
				if (!serialize::parse( result.m_struct, *m_resultreader, m_ctx, m_parsestk))
				{
					const char* err = m_ctx.getLastError();
					if (err)
					{
						LOG_ERROR << "error in transaction function result read (" << err << ")";
						return false;
					}
				}
				return true;
			}
		}
	}
}

bool TransactionFunctionMap::hasTransactionFunction( const std::string& name) const
{
	const char* ee = std::strchr( name.c_str(), ':');
	if (ee)
	{
		std::string nam( name.c_str(), ee-name.c_str());
		std::transform( nam.begin(), nam.end(), nam.begin(), ::tolower);
		return m_map.find( nam) != m_map.end();
	}
	else
	{
		std::string nam( name);
		std::transform( nam.begin(), nam.end(), nam.begin(), ::tolower);
		return m_map.find( nam) != m_map.end();
	}
}

void TransactionFunctionMap::defineTransactionFunction( const std::string& name, const TransactionFunction::Definition& f)
{
	defineObject( m_map, name, f);
}

bool TransactionFunctionMap::getTransactionFunction( const std::string& name, TransactionFunction& rt) const
{
	TransactionFunction::Definition def;
	if (!getObject( m_map, name, def)) return false;
	rt = def.create( name);
	return true;
}

DDLCompilerMap::DDLCompilerMap()
{
	ddl::CompilerInterfaceR simpleformCompiler( new ddl::SimpleFormCompiler());
	m_map[ simpleformCompiler->ddlname()] = simpleformCompiler;
}

void DDLCompilerMap::defineDDLCompiler( const std::string& name, const ddl::CompilerInterfaceR& f)
{
	defineObject( m_map, name, f);
}

bool DDLCompilerMap::getDDLCompiler( const std::string& name, ddl::CompilerInterfaceR& rt) const
{
	return getObject( m_map, name, rt);
}

static InputFilterClosure::ItemType fetchFailureResult( const InputFilter& ff)
{
	const char* msg;
	switch (ff.state())
	{
		case InputFilter::EndOfMessage:
			return InputFilterClosure::DoYield;

		case InputFilter::Error:
			msg = ff.getError();
			LOG_ERROR << "error in input filter (" << (msg?msg:"unknown") << ")";
			return InputFilterClosure::Error;

		case InputFilter::Open:
			LOG_DATA << "end of input";
			return InputFilterClosure::EndOfData;
	}
	LOG_ERROR << "illegal state in iterator";
	return InputFilterClosure::Error;
}

InputFilterClosure::ItemType InputFilterClosure::fetch( const char*& tag, unsigned int& tagsize, const char*& val, unsigned int& valsize)
{
	const void* element;
	std::size_t elementsize;

AGAIN:
	if (!m_inputfilter.get())
	{
		return EndOfData;
	}
	if (!m_inputfilter->getNext( m_type, element, elementsize))
	{
		return fetchFailureResult( *m_inputfilter);
	}
	else
	{
		switch (m_type)
		{
			case InputFilter::OpenTag:
				m_taglevel += 1;
				tag = (const char*)element;
				tagsize = elementsize;
				val = 0;
				valsize = 0;
				m_gotattr = false;
				return Data;

			case InputFilter::Value:
				if (m_gotattr)
				{
					tag = m_attrbuf.c_str();
					tagsize = m_attrbuf.size();
					m_gotattr = false;
				}
				else
				{
					tag = 0;
					tagsize = 0;
				}
				val = (const char*)element;
				valsize = elementsize;
				return Data;

			 case InputFilter::Attribute:
				m_attrbuf.clear();
				m_attrbuf.append( (const char*)element, elementsize);
				m_gotattr = true;
				goto AGAIN;

			 case InputFilter::CloseTag:
				tag = 0;
				tagsize = 0;
				val = 0;
				valsize = 0;
				m_gotattr = false;
				if (m_taglevel == 0)
				{
					return EndOfData;
				}
				else
				{
					m_taglevel -= 1;
					return Data;
				}
		}
	}
	LOG_ERROR << "illegal state in iterator";
	return Error;
}

Output::ItemType Output::print( const char* tag, unsigned int tagsize, const char* val, unsigned int valsize)
{
	if (!m_outputfilter.get())
	{
		LOG_ERROR << "no output sink defined (output ignored)";
		return Error;
	}
	try
	{
		if (tag)
		{
			if (val)
			{
				switch (m_state)
				{
					case 0:
						if (!m_outputfilter->print( OutputFilter::Attribute, tag, tagsize)) break;
						m_state ++;
					case 1:
						if (!m_outputfilter->print( OutputFilter::Value, val, valsize)) break;
						m_state ++;
					case 2:
						m_state = 0;
						return Data;
				}
				const char* err = m_outputfilter->getError();
				if (err)
				{
					LOG_ERROR << "error in output filter (" << err << ")";
					return Error;
				}
				return DoYield;
			}
			else
			{
				if (!m_outputfilter->print( OutputFilter::OpenTag, tag, tagsize))
				{
					const char* err = m_outputfilter->getError();
					if (err)
					{
						LOG_ERROR << "error in output filter open tag (" << err << ")";
						return Error;
					}
					return DoYield;
				}
				return Data;
			}
		}
		else if (val)
		{
			if (!m_outputfilter->print( OutputFilter::Value, val, valsize))
			{
				const char* err = m_outputfilter->getError();
				if (err)
				{
					LOG_ERROR << "error in output filter value (" << err << ")";
					return Error;
				}
				return DoYield;
			}
			return Data;
		}
		else
		{
			if (!m_outputfilter->print( OutputFilter::CloseTag, 0, 0))
			{
				const char* err = m_outputfilter->getError();
				if (err)
				{
					LOG_ERROR << "error in output filter close tag (" << err << ")";
					return Error;
				}
				return DoYield;
			}
			return Data;
		}
	}
	catch (std::bad_alloc)
	{
		LOG_ERROR << "out of memory in output filter";
		return Error;
	}
}





