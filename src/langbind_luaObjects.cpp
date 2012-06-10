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
#include "langbind/luaObjects.hpp"
#include "langbind/appObjects.hpp"
#include "langbind/appGlobalContext.hpp"
#include "langbind/luaDebug.hpp"
#include "langbind/luaException.hpp"
#include "filter/luafilter.hpp"
#include "logger-v1.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cstddef>
#include <boost/lexical_cast.hpp>
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include "langbind/luaDebug.hpp"

using namespace _Wolframe;
using namespace langbind;

namespace luaname
{
	static const char* Input = "wolframe.Input";
	static const char* Output = "wolframe.Output";
	static const char* Filter = "wolframe.Filter";
	static const char* DDLForm = "wolframe.DDLForm";
	static const char* GlobalContext = "wolframe.ctx";
	static const char* InputFilterClosure = "wolframe.InputFilterClosure";
	static const char* FormFunctionClosure = "wolframe.FormFunctionClosure";
}

namespace
{
template <class ObjectType>
const char* metaTableName()						{return 0;}
template <> const char* metaTableName<Input>()				{return luaname::Input;}
template <> const char* metaTableName<Output>()				{return luaname::Output;}
template <> const char* metaTableName<Filter>()				{return luaname::Filter;}
template <> const char* metaTableName<DDLForm>()			{return luaname::DDLForm;}
template <> const char* metaTableName<GlobalContext>()			{return luaname::GlobalContext;}
template <> const char* metaTableName<InputFilterClosure>()		{return luaname::InputFilterClosure;}
template <> const char* metaTableName<FormFunctionClosure>()		{return luaname::FormFunctionClosure;}
}//anonymous namespace

static const luaL_Reg empty_methodtable[ 1] =
{
	{0,0}
};

template <class ObjectType>
struct LuaObject :public ObjectType
{
	const char* m_name;
	LuaObject( const ObjectType& o)
		:ObjectType(o),m_name(metaTableName<ObjectType>()) {}
	LuaObject()
		:m_name(metaTableName<ObjectType>()) {}

	static int destroy( lua_State* ls)
	{
		LuaObject *THIS = (LuaObject*)lua_touserdata( ls, 1);
		if (THIS) THIS->~LuaObject();
		return 0;
	}

	static void createMetatable( lua_State* ls, lua_CFunction indexf, lua_CFunction newindexf, const luaL_Reg* mt)
	{
		luaL_newmetatable( ls, metaTableName<ObjectType>());
		lua_pushliteral( ls, "__index");
		if (indexf)
		{
			lua_pushcfunction( ls, indexf);
		}
		else
		{
			lua_pushvalue( ls, -2);
		}
		lua_rawset( ls, -3);

		lua_pushliteral( ls, "__newindex");
		if (newindexf)
		{
			lua_pushcfunction( ls, newindexf);
		}
		else
		{
			lua_pushvalue( ls, -2);
		}
		lua_rawset( ls, -3);

		lua_pushliteral( ls, "__gc");
		lua_pushcfunction( ls, destroy);
		lua_rawset( ls, -3);

		if (mt)
		{
			unsigned int ii;
			for (ii=0; mt[ii].name; ++ii)
			{
				lua_pushcfunction( ls, mt[ii].func);
				lua_setfield( ls, -2, mt[ii].name);
			}
		}
		lua_pop( ls, 1);
	}

	void* operator new (std::size_t num_bytes, lua_State* ls) throw (std::bad_alloc)
	{
		void* rt = lua_newuserdata( ls, num_bytes);
		if (rt == 0) throw std::bad_alloc();
		return rt;
	}

	void* operator new (std::size_t num_bytes, lua_State* ls, const char* mt) throw (std::bad_alloc)
	{
		void* rt = lua_newuserdata( ls, num_bytes);
		if (rt == 0) throw std::bad_alloc();
		luaL_getmetatable( ls, mt);
		lua_setmetatable( ls, -2);
		return rt;
	}

	/// \brief does nothing because the LUA garbage collector does the job.
	/// \warning CAUTION: DO NOT CALL THIS FUNCTION ! DOES NOT WORK ON MSVC 9.0. (The compiler links with the std delete)
	/// (just avoids C4291 warning)
	void operator delete (void *, lua_State*) {}
	/// ABa: got a C4291 warning here again, not sure about the whole construct here..
	/// an indicator could be that started the 'iprocd.exe' results in an error when
	/// opening the processor!
	/// PF: Overloading new and delete as defined in the C++ standard
	void operator delete (void *, lua_State*, const char*) {}

	template <class Orig>
	static void push_luastack( lua_State* ls, const Orig& o)
	{
		try
		{
			const char* mt = metaTableName<ObjectType>();
			(void*)new (ls,mt) LuaObject( o);
		}
		catch (const std::bad_alloc&)
		{
			luaL_error( ls, "memory allocation error in lua context");
		}
	}

	static void createGlobal( lua_State* ls, const char* name, const ObjectType& instance, const luaL_Reg* mt=0)
	{
		createMetatable( ls, 0, 0, mt);
		new (ls) LuaObject( instance);
		luaL_getmetatable( ls, metaTableName<ObjectType>());
		lua_setmetatable( ls, -2);
		lua_setglobal( ls, name);
	}

	static bool setGlobal( lua_State* ls, const char* name, const ObjectType& instance)
	{
		lua_getglobal( ls, name);
		LuaObject* obj = (LuaObject*) luaL_checkudata( ls, -1, metaTableName<ObjectType>());
		if (!obj) return false;
		*obj = instance;
		return true;
	}

	static ObjectType* getGlobal( lua_State* ls, const char* name)
	{
		lua_getglobal( ls, name);
		LuaObject* obj = (LuaObject*) luaL_checkudata( ls, -1, metaTableName<ObjectType>());
		if (!obj)
		{
			luaL_error( ls, "reserved global variable '%s' has been changed", name);
			return 0;
		}
		return obj;
	}

	static LuaObject* getSelf( lua_State* ls, const char* name, const char* method)
	{
		LuaObject* self;
		if (lua_gettop( ls) == 0 || (self=(LuaObject*)luaL_checkudata( ls, 1, metaTableName<ObjectType>())) == 0)
		{
			luaL_error( ls, "'%s' (metatable '%s') needs self parameter (%s:%s() instead of %s.%s())", name, metaTableName<ObjectType>(), name, method, name, method);
			return 0;
		}
		return self;
	}

	static LuaObject* get( lua_State* ls, int index)
	{
		LuaObject* rt = (LuaObject*) luaL_checkudata( ls, index, metaTableName<ObjectType>());
		return rt;
	}
};

template <class ObjectType>
static void setGlobalSingletonPointer( lua_State* ls, ObjectType* obj)
{
	luaL_newmetatable( ls, metaTableName<ObjectType>());
	lua_pushliteral( ls, "__index");
	lua_pushvalue( ls, -2);
	lua_rawset( ls, -3);
	lua_pushliteral( ls, "__newindex");
	lua_pushvalue( ls, -2);
	lua_rawset( ls, -3);
	ObjectType** objref = (ObjectType**)lua_newuserdata( ls, sizeof(ObjectType*));
	*objref = obj;
	luaL_getmetatable( ls, metaTableName<ObjectType>());
	lua_setmetatable( ls, -2);
	lua_setglobal( ls, metaTableName<ObjectType>());
}

template <class ObjectType>
static ObjectType* getGlobalSingletonPointer( lua_State* ls)
{
	lua_getglobal( ls, metaTableName<ObjectType>());
	ObjectType** objref = (ObjectType**) luaL_checkudata( ls, -1, metaTableName<ObjectType>());
	if (!objref)
	{
		luaL_error( ls, "reserved global variable '%s' has been changed", metaTableName<ObjectType>());
		return 0;
	}
	lua_pop( ls, 1);
	return *objref;
}


template <class Object>
static int function__LuaObject__index( lua_State* ls)
{
	Object* obj = LuaObject<Object>::getSelf( ls, metaTableName<Object>(), "__index");
	const char* key = lua_tostring( ls, 2);
	if (!key) luaL_error( ls, "%s __index called with an invalid argument 2 (key)", metaTableName<Object>());

	bool rt = false;
	bool overfl = false;
	char valbuf[ 256];
	try
	{
		std::string val;
		rt = obj->getValue( key, val);
		std::size_t nn = (overfl=(val.size() >= sizeof( valbuf)))?(sizeof( valbuf)-1):val.size();
		std::memcpy( valbuf, val.c_str(), nn);
		valbuf[ nn] = 0;
	}
	catch (const std::bad_alloc&)
	{
		return luaL_error( ls, "out of memory calling %s __index", metaTableName<Object>());
	}
	catch (const std::exception& e)
	{
		return luaL_error( ls, "%s __index called with illegal value (%s)", metaTableName<Object>(), e.what());
	}
	if (rt)
	{
		if (overfl)
		{
			return luaL_error( ls, "%s __index variable size exceeds maximum size (%u)", metaTableName<Object>(), sizeof(valbuf));
		}

		lua_pushstring( ls, valbuf);
		return 1;
	}
	else
	{
		return luaL_error( ls, "%s __index called with unknown variable name", metaTableName<Object>());
	}
	return 0;
}

template <class Object>
static int function__LuaObject__newindex( lua_State* ls)
{
	Object* obj = LuaObject<Object>::getSelf( ls, metaTableName<Object>(), "__newindex");
	const char* key = lua_tostring( ls, 2);
	if (!key) luaL_error( ls, "%s __newindex called with invalid argument 2 (key)", metaTableName<Object>());
	const char* val = 0;
	int tp = lua_type( ls, 3);

	if (tp == LUA_TBOOLEAN)
	{
		val = lua_toboolean( ls, 3)?"true":"false";
	}
	else
	{
		val = lua_tostring( ls, 3);
	}
	if (!val) luaL_error( ls, "%s __newindex called with invalid argument 3 (value)", metaTableName<Object>());

	try
	{
		if (!obj->setValue( key, val))
		{
			luaL_error( ls, "%s __newindex called with unknown variable name '%s'", metaTableName<Object>(), key);
		}
	}
	catch (const std::bad_alloc&)
	{
		return luaL_error( ls, "out of memory calling %s __newindex", metaTableName<Object>());
	}
	catch (const std::exception& e)
	{
		return luaL_error( ls, "%s __newindex called with illegal value (%s)", metaTableName<Object>(), e.what());
	}
	return 0;
}

static void pushItem( lua_State* ls, const char* item, unsigned int itemsize)
{
	if (item)
	{
		lua_pushlstring( ls, item, (std::size_t)itemsize);
		lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
	}
	else
	{
		lua_pushboolean( ls, 0);
	}
}

static int function_inputFilter( lua_State* ls)
{
	const char* item[2];
	unsigned int itemsize[2];
	InputFilterClosure* closure = (InputFilterClosure*)lua_touserdata( ls, lua_upvalueindex( 1));

	try
	{
		switch (closure->fetch( item[1]/*tag*/, itemsize[1], item[0]/*val*/, itemsize[0]))
		{
			case InputFilterClosure::DoYield:
				lua_yieldk( ls, 0, 1, function_inputFilter);

			case InputFilterClosure::EndOfData:
				return 0;

			case InputFilterClosure::Error:
				luaL_error( ls, "error in iterator");
				return 0;

			case InputFilterClosure::Data:
				pushItem( ls, item[0], itemsize[0]);
				pushItem( ls, item[1], itemsize[1]);
				return 2;
		}
	}
	catch (const std::bad_alloc&)
	{
		return luaL_error( ls, "out of memory calling input filter fetch");
	}
	catch (const std::exception& e)
	{
		return luaL_error( ls, "got exception in input filter fetch: (%s)", e.what());
	}
	return luaL_error( ls, "illegal state produced by input filter fetch");
}


static bool get_operand_TypedOutputFilter( lua_State* ls, int idx, langbind::TypedOutputFilterR& flt)
{
	switch (lua_type( ls, idx))
	{
		case LUA_TTABLE:
			flt.reset( new langbind::LuaOutputFilter( ls));
			return true;
		case LUA_TUSERDATA:
		default:
			return false;
	}
}

static bool get_operand_TypedInputFilter( lua_State* ls, int idx, langbind::TypedInputFilterR& flt)
{
	switch (lua_type( ls, idx))
	{
		case LUA_TTABLE:
			flt.reset( new langbind::LuaInputFilter( ls));
			return true;
		case LUA_TUSERDATA:
		default:
			return false;
	}
}

static int push_result_TypedOutputFilter( lua_State* ls, const langbind::TypedOutputFilterR& flt)
{
	langbind::LuaOutputFilter* lf = dynamic_cast<langbind::LuaOutputFilter*>( flt.get());
	if (lf)
	{
		if (!lua_istable( ls, -1))
		{
			luaL_error( ls, "result is not a lua table as expected");
			return 0;
		}
		/* result table already on the stack */
		return 1;
	}
	luaL_error( ls, "runtime error. cannot fetch result");
	return 0;
}

static int function_formfunction_call( lua_State* ls)
{
	FormFunctionClosure* closure = (FormFunctionClosure*)lua_touserdata( ls, lua_upvalueindex( 1));
	try
	{
		int ctx;
		if (lua_getctx( ls, &ctx) != LUA_YIELD)
		{
			if (lua_gettop( ls) != 2)
			{
				return luaL_error( ls, "two arguments expected for call of formfunction");
			}
			else
			{
				langbind::TypedInputFilterR inp;
				if (!get_operand_TypedInputFilter( ls, 1, inp))
				{
					luaL_error( ls, "error in form function call (%s)", "1st argument is not a table, form or filter");
				}
				langbind::TypedOutputFilterR outp;
				if (!get_operand_TypedOutputFilter( ls, 2, outp))
				{
					return luaL_error( ls, "error in form function call (%s)", "2nd argument is not a table, form or filter");
				}
				closure->init( inp, outp);
			}
		}
		langbind::FormFunction::CallResult res = closure->call();
		switch (res)
		{
			case langbind::FormFunction::Yield:
				lua_yieldk( ls, 0, 1, function_formfunction_call);

			case langbind::FormFunction::Ok:
				return push_result_TypedOutputFilter( ls, closure->outputfilter());

			case langbind::FormFunction::Error:
				return luaL_error( ls, "error in form function call (%s)", closure->getLastError());
		}
	}
	catch (const std::bad_alloc&)
	{
		return luaL_error( ls, "out of memory calling form function call");
	}
	catch (const std::exception& e)
	{
		return luaL_error( ls, "got exception in form function call: (%s)", e.what());
	}
	return luaL_error( ls, "illegal state produced by form function call");
}

static const char* get_printop( lua_State* ls, int index, std::size_t& size)
{
	const char* rt = 0;
	if (lua_isnil( ls, index) || (lua_isboolean( ls, index) && !lua_toboolean( ls, index))) {}
	else if ((rt=lua_tolstring( ls, index, &size)) == 0)
	{
		const char* tn = lua_typename( ls, lua_type( ls, index));
		luaL_error( ls, "invalid type (%s) of argument %d (convertable to string or nil or false expected)", tn?tn:"unknown", index-1);
	}
	return rt;
}

static int function_output_print( lua_State* ls)
{
	const char* msg;
	const char* item[2] = {0,0};
	std::size_t itemsize[2] = {0,0};

	Output* output = LuaObject<Output>::getSelf( ls, "output", "print");
	if (lua_gettop( ls) == 1)
	{}
	else if (lua_gettop( ls) == 2)
	{
		item[0] = get_printop( ls, 2, itemsize[0]);
	}
	else if (lua_gettop( ls) == 3)
	{
		item[0] = get_printop( ls, 2, itemsize[0]);
		item[1] = get_printop( ls, 3, itemsize[1]);
	}
	else
	{
		return luaL_error( ls, "too many arguments in call of output:print");
	}

	try
	{
		switch (output->print( item[1]/*tag*/, itemsize[1], item[0]/*val*/, itemsize[0]))
		{
			case Output::DoYield:
				lua_yieldk( ls, 0, 1, function_output_print);

			case Output::Error:
				msg = output->outputfilter()->getError();
				return luaL_error( ls, "error in output:print (%s)", msg?msg:"unknown");

			case Output::Data:
				return 0;
		}
	}
	catch (const std::bad_alloc&)
	{
		return luaL_error( ls, "out of memory calling output:print");
	}
	catch (const std::exception& e)
	{
		return luaL_error( ls, "got exception in output:print: (%s)", e.what());
	}
	return luaL_error( ls, "illegal state produced by output:print");
}

static int function_output_opentag( lua_State* ls)
{
	const char* msg;
	const char* tag = 0;
	std::size_t tagsize = 0;

	Output* output = LuaObject<Output>::getSelf( ls, "output", "opentag");
	if (lua_gettop( ls) == 1)
	{
		return luaL_error( ls, "output:opentag called without tag name as argument");
	}
	else if (lua_gettop( ls) == 2)
	{
		tag = get_printop( ls, 2, tagsize);
	}
	else if (lua_gettop( ls) > 2)
	{
		return luaL_error( ls, "output:opentag called with too many arguments");
	}
	try
	{
		switch (output->print( tag, tagsize, 0/*val*/, 0))
		{
			case Output::DoYield:
				lua_yieldk( ls, 0, 1, function_output_opentag);

			case Output::Error:
				msg = output->outputfilter()->getError();
				return luaL_error( ls, "error in output:opentag (%s)", msg?msg:"unknown");

			case Output::Data:
				return 0;
		}
	}
	catch (const std::bad_alloc&)
	{
		return luaL_error( ls, "out of memory calling output:opentag");
	}
	catch (const std::exception& e)
	{
		return luaL_error( ls, "got exception in output:opentag: (%s)", e.what());
	}
	return luaL_error( ls, "illegal state produced by output:opentag");
}

static int function_output_closetag( lua_State* ls)
{
	const char* msg;

	Output* output = LuaObject<Output>::getSelf( ls, "output", "closetag");
	if (lua_gettop( ls) > 1)
	{
		return luaL_error( ls, "output:closetag called with too many arguments. no arguments expected");
	}
	try
	{
		switch (output->print( 0/*tag*/, 0, 0/*val*/, 0))
		{
			case Output::DoYield:
				lua_yieldk( ls, 0, 1, function_output_closetag);

			case Output::Error:
				msg = output->outputfilter()->getError();
				return luaL_error( ls, "error in output:closetag (%s)", msg?msg:"unknown");

			case Output::Data:
				return 0;
		}
	}
	catch (const std::bad_alloc&)
	{
		return luaL_error( ls, "out of memory calling output:closetag");
	}
	catch (const std::exception& e)
	{
		return luaL_error( ls, "got exception in output:closetag: (%s)", e.what());
	}
	return luaL_error( ls, "illegal state produced by output:closetag");
}

static int function_filter( lua_State* ls)
{
	unsigned int nn = lua_gettop( ls);
	if (nn == 0) return luaL_error( ls, "too few arguments for function 'filter'");
	if (nn > 1) return luaL_error( ls, "too many arguments for function 'filter'");
	if (!lua_isstring( ls, 1))
	{
		return luaL_error( ls, "invalid type of argument (string expected)");
	}
	const char* name = lua_tostring( ls, 1);
	GlobalContext* ctx = getGlobalSingletonPointer<GlobalContext>( ls);
	if (!ctx)
	{
		return luaL_error( ls, "internal error. function 'filter' got no global context");
	}
	Filter flt;
	try
	{
		if (!ctx->getFilter( name, flt))
		{
			return luaL_error( ls, "could not get filter '%s'", name);
		}
	}
	catch (const std::exception& e)
	{
		return luaL_error( ls, "got exception in function 'filter': (%s)", e.what());
	}
	LuaObject<Filter>::push_luastack( ls, flt);
	return 1;
}

static int function_formfunction( lua_State* ls)
{
	unsigned int nn = lua_gettop( ls);
	if (nn == 0) return luaL_error( ls, "too few arguments for function 'formfunction'");
	if (nn > 1) return luaL_error( ls, "too many arguments for function 'formfunction'");
	if (!lua_isstring( ls, 1))
	{
		return luaL_error( ls, "invalid type of argument (string expected)");
	}
	const char* name = lua_tostring( ls, 1);
	GlobalContext* ctx = getGlobalSingletonPointer<GlobalContext>( ls);
	if (!ctx)
	{
		return luaL_error( ls, "internal error. function 'formfunction' got no global context");
	}
	FormFunction func;
	try
	{
		if (!ctx->getFormFunction( name, func))
		{
			return luaL_error( ls, "could not get form function '%s'", name);
		}
	}
	catch (const std::exception& e)
	{
		return luaL_error( ls, "got exception in function 'formfunction': (%s)", e.what());
	}
	LuaObject<FormFunctionClosure>::push_luastack( ls, FormFunctionClosure( func));
	lua_pushcclosure( ls, function_formfunction_call, 1);
	return 1;
}

static int function_input_as( lua_State* ls)
{
	Input* input = LuaObject<Input>::getSelf( ls, "input", "as");
	if (lua_gettop( ls) != 2)
	{
		luaL_error( ls, "filter type value as parameter of method 'input:as' expected");
	}
	LuaObject<Filter>* filter = LuaObject<Filter>::get( ls, 2);

	if (!filter)
	{
		const char* tn = lua_typename( ls, lua_type( ls, 2));
		luaL_error( ls, "filter type value expected as first argument of input:as instead of %s", tn?tn:"UNKNOWN");
	}
	try
	{
		langbind::InputFilter* ff = 0;
		if (filter->inputfilter().get())
		{
			ff = filter->inputfilter()->copy();
			if (input->inputfilter().get())
			{
				// assign the rest of the input to the new filter attached
				const void* chunk;
				std::size_t chunksize;
				bool chunkend;
				input->inputfilter()->getRest( chunk, chunksize, chunkend);
				ff->putInput( chunk, chunksize, chunkend);
			}
			input->inputfilter().reset( ff);
		}
		else
		{
			luaL_error( ls, "input:as called with a filter with undefined input");
		}
	}
	catch (const std::bad_alloc&)
	{
		return luaL_error( ls, "out of memory calling input:as");
	}
	catch (const std::exception& e)
	{
		return luaL_error( ls, "got exception in input:as: (%s)", e.what());
	}
	return 0;
}

static int function_output_as( lua_State* ls)
{
	Output* output = LuaObject<Output>::getSelf( ls, "output", "as");
	if (lua_gettop( ls) != 2)
	{
		luaL_error( ls, "filter type value as parameter of method 'output:as' expected");
	}
	Filter* filter = LuaObject<Filter>::get( ls, 2);
	if (!filter)
	{
		const char* tn = lua_typename( ls, lua_type( ls, 2));
		luaL_error( ls, "filter type value expected as first argument of output:as instead of %s", tn?tn:"UNKNOWN");
	}
	try
	{
		langbind::OutputFilter* ff = 0;
		if (filter->outputfilter().get())
		{
			ff = filter->outputfilter()->copy();
			if (output->outputfilter().get())
			{
				ff->assignState( *output->outputfilter());
			}
		}
		else
		{
			luaL_error( ls, "output:as called with a filter with undefined output");
		}
		output->outputfilter().reset( ff);
	}
	catch (const std::bad_alloc&)
	{
		return luaL_error( ls, "out of memory calling output:as");
	}
	catch (const std::exception& e)
	{
		return luaL_error( ls, "got exception in output:as: (%s)", e.what());
	}
	return 0;
}

static int function_input_get( lua_State* ls)
{
	Input* input = LuaObject<Input>::getSelf( ls, "input", "get");
	if (lua_gettop( ls) != 1)
	{
		luaL_error( ls, "no arguments expected for input:get");
	}
	if (!input->inputfilter().get())
	{
		return luaL_error( ls, "no filter defined for input with input:as");
	}
	LuaObject<InputFilterClosure>::push_luastack( ls, input->inputfilter());
	lua_pushcclosure( ls, function_inputFilter, 1);
	return 1;
}

static int function_yield( lua_State* ls)
{
	int ii,nn=lua_gettop( ls);
	for (ii=0; ii<nn; ii++) lua_pushvalue( ls, ii);
	return lua_yieldk( ls, ii, 1, function_yield);
}

static const luaL_Reg input_methodtable[ 3] =
{
	{"as",&function_input_as},
	{"get",&function_input_get},
	{0,0}
};

static const luaL_Reg output_methodtable[ 5] =
{
	{"as",&function_output_as},
	{"print",&function_output_print},
	{"opentag",function_output_opentag},
	{"closetag",function_output_closetag},
	{0,0}
};

static int function_printlog( lua_State *ls)
{
	/* first parameter maps to a log level, rest gets printed depending on
	 * whether it's a string or a number
	 */
	int ii,nn = lua_gettop(ls);
	if (nn <= 0)
	{
		luaL_error( ls, "no arguments passed to 'printlog'");
		return 0;
	}
	const char *logLevel = luaL_checkstring( ls, 1);
	std::string logmsg;

	for (ii=2; ii<=nn; ii++)
	{
		if (!getDescription( ls, ii, logmsg))
		{
			luaL_error( ls, "failed to map 'printLog' arguments to a string");
			return 0;
		}
	}
	_Wolframe::log::LogLevel::Level lv = _Wolframe::log::LogLevel::strToLogLevel( logLevel);
	if (lv == _Wolframe::log::LogLevel::LOGLEVEL_UNDEFINED)
	{
		luaL_error( ls, "'printLog' called with undefined loglevel '%s' as first argument", logLevel);
		return 0;
	}
	else
	{
		_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( lv )
			<< _Wolframe::log::LogComponent::LogLua
			<< logmsg;
	}
	return 0;
}

LuaScript::LuaScript( const std::string& path_)
	:m_path(path_)
{
	char buf;
	std::fstream ff;
	ff.open( path_.c_str(), std::ios::in);
	while (ff.read( &buf, sizeof(buf)))
	{
		m_content.push_back( buf);
	}
	if ((ff.rdstate() & std::ifstream::eofbit) == 0)
	{
		LOG_ERROR << "failed to read lua script from file: '" << path_ << "'";
		throw std::runtime_error( "read lua script from file");
	}
	ff.close();
}

LuaScriptInstance::LuaScriptInstance( const LuaScript* script_)
	:m_ls(0),m_thread(0),m_threadref(0),m_script(script_)
{
	m_ls = luaL_newstate();
	if (!m_ls) throw std::runtime_error( "failed to create lua state");

	LuaExceptionHandlerScope luaThrows(m_ls);
	{
		// create thread and prevent garbage collecting of it (http://permalink.gmane.org/gmane.comp.lang.lua.general/22680)
		m_thread = lua_newthread( m_ls);
		lua_pushvalue( m_ls, -1);
		m_threadref = luaL_ref( m_ls, LUA_REGISTRYINDEX);

		if (luaL_loadbuffer( m_ls, m_script->content().c_str(), m_script->content().size(), m_script->path().c_str()))
		{
			std::ostringstream buf;
			buf << "Failed to load script '" << m_script->path() << "':" << lua_tostring( m_ls, -1);
			throw std::runtime_error( buf.str());
		}
		// open standard lua libraries
		luaL_openlibs( m_ls);

		// register logging function already here because then it can be used in the script initilization part
		lua_pushcfunction( m_ls, &function_printlog);
		lua_setglobal( m_ls, "printlog");

		// open additional libraries defined for this script
		std::vector<LuaScript::Module>::const_iterator ii=m_script->modules().begin(), ee=m_script->modules().end();
		for (;ii!=ee; ++ii)
		{
			if (ii->m_initializer( m_ls))
			{
				std::ostringstream buf;
				buf << "module '" << ii->m_name << "' initialization failed: " << lua_tostring( m_ls, -1);
				throw std::runtime_error( buf.str());
			}
		}

		// call main, we may have to initialize LUA modules there
		if (lua_pcall( m_ls, 0, LUA_MULTRET, 0) != 0)
		{
			std::ostringstream buf;
			buf << "Unable to call main entry of script: " << lua_tostring( m_ls, -1 );
			throw std::runtime_error( buf.str());
		}
	}
}

LuaScriptInstance::~LuaScriptInstance()
{
	if (m_ls)
	{
		luaL_unref( m_ls, LUA_REGISTRYINDEX, m_threadref);
		lua_close( m_ls);
	}
}

LuaFunctionMap::~LuaFunctionMap()
{
	std::vector<LuaScript*>::iterator ii=m_ar.begin(),ee=m_ar.end();
	while (ii != ee)
	{
		delete *ii;
		++ii;
	}
}

void LuaFunctionMap::defineLuaFunction( const std::string& name, const LuaScript& script)
{
	std::string nam( name);
	std::transform( nam.begin(), nam.end(), nam.begin(), ::tolower);
	{
		std::map<std::string,std::size_t>::const_iterator ii=m_procmap.find( nam),ee=m_procmap.end();
		if (ii != ee)
		{
			std::ostringstream buf;
			buf << "Duplicate definition of function '" << nam << "'";
			throw std::runtime_error( buf.str());
		}
	}
	std::size_t scriptId;
	std::map<std::string,std::size_t>::const_iterator ii=m_pathmap.find( script.path()),ee=m_pathmap.end();
	if (ii != ee)
	{
		scriptId = ii->second;
	}
	else
	{
		scriptId = m_ar.size();
		m_ar.push_back( new LuaScript( script));	//< load its content from file
		LuaScriptInstance( m_ar.back());		//< check, if it can be compiled
		m_pathmap[ script.path()] = scriptId;
	}
	m_procmap[ nam] = scriptId;
}

bool LuaFunctionMap::getLuaScriptInstance( const std::string& procname, LuaScriptInstanceR& rt) const
{
	std::string nam( procname);
	std::transform( nam.begin(), nam.end(), nam.begin(), ::tolower);

	std::map<std::string,std::size_t>::const_iterator ii=m_procmap.find( nam),ee=m_procmap.end();
	if (ii == ee) return false;
	rt = LuaScriptInstanceR( new LuaScriptInstance( m_ar[ ii->second]));
	return true;
}

bool LuaFunctionMap::initLuaScriptInstance( LuaScriptInstance* lsi, const Input& input_, const Output& output_)
{
	GlobalContext* gc = dynamic_cast<GlobalContext*>( this);
	if (!gc)
	{
		LOG_ERROR << "cannot get global context via RTTI from Lua function map";
		return false;
	}
	lua_State* ls = lsi->ls();
	try
	{
		LuaExceptionHandlerScope luaThrows(ls);
		{
			LuaObject<Input>::createGlobal( ls, "input", input_, input_methodtable);
			LuaObject<Output>::createGlobal( ls, "output", output_, output_methodtable);
			LuaObject<Filter>::createMetatable( ls, &function__LuaObject__index<Filter>, &function__LuaObject__newindex<Filter>, 0);
			LuaObject<InputFilterClosure>::createMetatable( ls, 0, 0, 0);
			setGlobalSingletonPointer<GlobalContext>( ls, gc);
			lua_pushcfunction( ls, &function_yield);
			lua_setglobal( ls, "yield");
			lua_pushcfunction( ls, &function_filter);
			lua_setglobal( ls, "filter");
			lua_pushcfunction( ls, &function_formfunction);
			lua_setglobal( ls, "formfunction");
		}
		return true;
	}
	catch (const std::exception& e)
	{
		LOG_ERROR << "exception intializing lua script instance. " << e.what();
	}
	return false;
}

