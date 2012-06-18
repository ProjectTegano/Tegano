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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file langbind/pipe.hpp
///\brief Implementation for a pipe (istream|ostream) through wolframe mappings like filters, forms, functions

#include "logger-v1.hpp"
#include "langbind/appGlobalContext.hpp"
#include "langbind/iostreamfilter.hpp"
#include "serialize/ddl/filtermapDDLParse.hpp"
#include "serialize/ddl/filtermapDDLPrint.hpp"
#include "filter/token_filter.hpp"
#include "filter/serializefilter.hpp"
#if WITH_LUA
#include "cmdbind/luaCommandHandler.hpp"
#include "langbind/luaObjects.hpp"
#endif
#include <boost/algorithm/string.hpp>
#include <string>
#include <fstream>
#include <iostream>

using namespace _Wolframe;
using namespace langbind;

static langbind::Filter getFilter( langbind::GlobalContext* gc, const std::string& ifl, const std::string& ofl)
{
	langbind::Filter rt;
	if (boost::iequals( ofl, ifl))
	{
		if (!gc->getFilter( ifl.c_str(), rt))
		{
			LOG_ERROR << "unknown filter '" << ifl << "'";
			return rt;
		}
	}
	else
	{
		langbind::Filter in;
		langbind::Filter out;
		if (!gc->getFilter( ifl.c_str(), in))
		{
			LOG_ERROR << "unknown input filter '" << ofl << "'";
			return rt;
		}
		if (!gc->getFilter( ofl.c_str(), out))
		{
			LOG_ERROR << "unknown output filter '" << ofl << "'";
			return rt;
		}
		rt = langbind::Filter( in.inputfilter(), out.outputfilter());
	}
	return rt;
}

static bool readInput( char* buf, unsigned int bufsize, std::istream& is, InputFilter& iflt)
{
	std::size_t pp = 0;
	while (pp < bufsize && !is.eof())
	{
		is.read( buf+pp, sizeof(char));
		if (!is.eof()) ++pp;
	}
	iflt.putInput( buf, pp, is.eof());
	iflt.setState( InputFilter::Open);
	return (!is.eof() || pp);
}

static void writeOutput( char* buf, unsigned int size, std::ostream& os, OutputFilter& oflt)
{
	os.write( buf, oflt.getPosition());
	oflt.setOutputBuffer( buf, size);
	oflt.setState( OutputFilter::Open);
}

struct BufferStruct
{
	boost::shared_ptr<char> mem;
	char* inbuf;
	char* outbuf;
	std::size_t insize;
	std::size_t outsize;

	BufferStruct( std::size_t ib, std::size_t ob)
		:mem( (char*)std::calloc( ib+ob, 1), std::free),insize(ib),outsize(ob)
	{
		inbuf = mem.get();
		outbuf = mem.get()+ib;
	}
};

static bool processIO( BufferStruct& buf, langbind::InputFilter* iflt, langbind::OutputFilter* oflt, std::istream& is, std::ostream& os)
{
	const char* errmsg;
	if (!iflt || !oflt)
	{
		LOG_ERROR << "Error lost filter";
		return false;
	}
	switch (iflt->state())
	{
		case InputFilter::Open:
			break;

		case InputFilter::EndOfMessage:
			return readInput( buf.inbuf, buf.insize, is, *iflt);

		case InputFilter::Error:
			errmsg = iflt->getError();
			LOG_ERROR << "Error in input filter: " << (errmsg?errmsg:"unknown");
			return false;
	}
	switch (oflt->state())
	{
		case OutputFilter::Open:
			return false;

		case OutputFilter::EndOfBuffer:
			writeOutput( buf.outbuf, buf.outsize, os, *oflt);
			return true;

		case OutputFilter::Error:
			errmsg = oflt->getError();
			LOG_ERROR << "Error in output filter: " << (errmsg?errmsg:"unknown");
			return false;
	}
	return true;
}

template <class Obj>
static void logError( const char* title, const Obj& obj)
{
	const char* pp = obj.getLastErrorPos();
	const char* ee = obj.getLastError();
	if (pp)
	{
		LOG_ERROR << "error at '" << pp << "' " << title << " (" << " (" << (ee?ee:"unknown") << ")";
	}
	else
	{
		LOG_ERROR << "error " << title << " (" << (ee?ee:"unknown") << ")";
	}
}

bool _Wolframe::langbind::iostreamfilter( const std::string& proc, const std::string& ifl, std::size_t ib, const std::string& ofl, std::size_t ob, std::istream& is, std::ostream& os)
{
	langbind::GlobalContext* gc = langbind::getGlobalContext();
	langbind::FilterFactoryR tf( new langbind::TokenFilterFactory());
	gc->defineFilter( "token", tf);

	langbind::Filter flt = getFilter( gc, ifl, ofl);
	if (!flt.inputfilter().get() || !flt.outputfilter().get()) return false;

	BufferStruct buf( ib, ob);
	flt.outputfilter()->setOutputBuffer( buf.outbuf, buf.outsize);

	if (proc.size() == 0 || proc == "-")
	{
		const void* elem;
		std::size_t elemsize;
		InputFilter::ElementType etype;

		for (;;)
		{
			if (!flt.inputfilter().get()->getNext( etype, elem, elemsize))
			{
				if (!processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os)) goto _END_FILTER_LOOP;
				continue;
			}
			while (!flt.outputfilter().get()->print( etype, elem, elemsize))
			{
				if (!processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os)) goto _END_FILTER_LOOP;
			}
		}
		_END_FILTER_LOOP:
		writeOutput( buf.outbuf, buf.outsize, os, *flt.outputfilter());
		return	flt.inputfilter()->state() == InputFilter::Open
			&& flt.outputfilter()->state() == OutputFilter::Open;
	}
#if WITH_LUA
	{
		LuaScriptInstanceR sc;
		if (gc->getLuaScriptInstance( proc, sc))
		{
			if (!gc->initLuaScriptInstance( sc.get(), flt.inputfilter(), flt.outputfilter()))
			{
				LOG_ERROR << "error initializing lua script";
				return false;
			}
			lua_getglobal( sc->thread(), proc.c_str());
			int rt = lua_resume( sc->thread(), NULL, 0);
			while (rt == LUA_YIELD)
			{
				if (processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os)
				|| (flt.inputfilter()->state() == InputFilter::Open && flt.outputfilter()->state() == OutputFilter::Open))
				{
					rt = lua_resume( sc->thread(), NULL, 0);
				}
			}
			if (rt == LUA_OK)
			{
				writeOutput( buf.outbuf, buf.outsize, os, *flt.outputfilter());
				return true;
			}
			else
			{
				const char* msg = lua_tostring( sc->thread(), -1);
				LOG_ERROR << "error in lua script: '" << msg << "'";
				return false;
			}
		}
	}
#endif
	{
		FormFunction func;
		if (gc->getFormFunction( proc.c_str(), func))
		{
			flt.inputfilter()->setValue( "empty", "false");
			langbind::TypedInputFilterR inp( new langbind::SerializeInputFilter( flt.inputfilter().get()));
			langbind::TypedOutputFilterR outp( new langbind::SerializeOutputFilter( flt.outputfilter().get()));
			langbind::FormFunctionClosure closure( func);
			closure.init( inp);
			langbind::FormFunctionClosure::CallResult callrt = closure.call();
			while (callrt == langbind::FormFunctionClosure::Yield)
			{
				if (processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os)
				|| (flt.inputfilter()->state() == InputFilter::Open && flt.outputfilter()->state() == OutputFilter::Open))
				{
					callrt = closure.call();
				}
				else
				{
					break;
				}
			}
			if (callrt == langbind::FormFunctionClosure::Ok)
			{
				langbind::FormFunctionResult res = closure.result();
				res.init( outp);
				langbind::FormFunctionResult::CallResult fetchrt = res.fetch();
				while (fetchrt == langbind::FormFunctionResult::Yield)
				{
					if (processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os)
					|| (flt.inputfilter()->state() == InputFilter::Open && flt.outputfilter()->state() == OutputFilter::Open))
					{
						fetchrt = res.fetch();
					}
					else
					{
						break;
					}
				}
				if (fetchrt == langbind::FormFunctionResult::Ok)
				{
					writeOutput( buf.outbuf, buf.outsize, os, *flt.outputfilter());
					return true;
				}
				else
				{
					logError( "fetching form function result", res);
					return false;
				}
			}
			else
			{
				logError( "in form function call", closure);
				return false;
			}
		}
	}
	{
		DDLFormR df;
		if (gc->getForm( proc.c_str(), df))
		{
			flt.inputfilter()->setValue( "empty", "false");
			langbind::TypedInputFilterR inp( new langbind::SerializeInputFilter( flt.inputfilter().get()));
			langbind::TypedOutputFilterR outp( new langbind::SerializeOutputFilter( flt.outputfilter().get()));
			DDLFormFill closure( df, inp, serialize::Context::ValidateAttributes);

			langbind::DDLFormFill::CallResult callrt = closure.call();
			while (callrt == langbind::DDLFormFill::Yield)
			{
				if (processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os)
				|| (flt.inputfilter()->state() == InputFilter::Open))
				{
					callrt = closure.call();
				}
				else
				{
					break;
				}
			}
			if (callrt == langbind::DDLFormFill::Ok)
			{
				langbind::DDLFormPrint res( df, outp, serialize::Context::None);
				langbind::DDLFormPrint::CallResult fetchrt = res.fetch();
				while (fetchrt == langbind::DDLFormPrint::Yield)
				{
					if (processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os)
					|| (flt.outputfilter()->state() == OutputFilter::Open))
					{
						fetchrt = res.fetch();
					}
					else
					{
						break;
					}
				}
				if (fetchrt == langbind::DDLFormPrint::Ok)
				{
					writeOutput( buf.outbuf, buf.outsize, os, *flt.outputfilter());
					return true;
				}
				else
				{
					logError( "fetching DDL form map result", res);
					return false;
				}
			}
			else
			{
				logError( "in DDL form map", closure);
				return false;
			}
		}
	}
	{
		TransactionFunction func;
		if (gc->getTransactionFunction( proc.c_str(), func))
		{
			flt.inputfilter()->setValue( "empty", "false");
			langbind::TypedInputFilterR inp( new langbind::SerializeInputFilter( flt.inputfilter().get()));
			langbind::TypedOutputFilterR outp( new langbind::SerializeOutputFilter( flt.outputfilter().get()));
			langbind::TransactionFunctionClosure closure( proc, func);
			closure.init( inp);
			langbind::TransactionFunctionClosure::CallResult callrt = closure.call();
			while (callrt == langbind::TransactionFunctionClosure::Yield)
			{
				if (processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os)
				|| (flt.inputfilter()->state() == InputFilter::Open && flt.outputfilter()->state() == OutputFilter::Open))
				{
					callrt = closure.call();
				}
				else
				{
					break;
				}
			}
			if (callrt == langbind::TransactionFunctionClosure::Ok)
			{
				langbind::TransactionFunctionResult res = closure.result();
				res.init( outp);
				langbind::TransactionFunctionResult::CallResult fetchrt = res.fetch();
				while (fetchrt == langbind::TransactionFunctionResult::Yield)
				{
					if (processIO( buf, flt.inputfilter().get(), flt.outputfilter().get(), is, os)
					|| (flt.inputfilter()->state() == InputFilter::Open && flt.outputfilter()->state() == OutputFilter::Open))
					{
						fetchrt = res.fetch();
					}
					else
					{
						break;
					}
				}
				if (fetchrt == langbind::TransactionFunctionResult::Ok)
				{
					writeOutput( buf.outbuf, buf.outsize, os, *flt.outputfilter());
					return true;
				}
				else
				{
					const char* ee = res.getLastError();
					LOG_ERROR << "error fetching transaction function result (" << (ee?ee:"unknown") << ")";
					return false;
				}
			}
			else
			{
				const char* ee = closure.getLastError();
				LOG_ERROR << "error in transaction function call (" << (ee?ee:"unknown") << ")";
				return false;
			}
		}
	}
	LOG_ERROR << "mapping command not found: '" << proc.c_str() << "'";
	return false;
}


