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
/// \file luafilter.cpp
/// \brief Implementation of lua filters (serialization/deserialization of lua tables)
#include "luafilter.hpp"
#include "luaObjectTemplate.hpp"
#include "luaDebug.hpp"
#include "logger/logger-v1.hpp"
#include "utils/printFormats.hpp"
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <cstring>
#include <string>
#include <cmath>

extern "C" {
	#include <lualib.h>
	#include <lauxlib.h>
	#include <lua.h>
}

using namespace _Wolframe;
using namespace langbind;

static std::string get_lua_elem( lua_State* ls, int idx)
{
	switch (lua_type( ls, idx))
	{
		case LUA_TNIL:		return "nil";
		case LUA_TNUMBER:	return std::string("V(") + boost::lexical_cast<std::string>( lua_tonumber( ls, idx)) + ")";
		case LUA_TBOOLEAN:	return std::string("V(") + (lua_toboolean(ls,idx)?"true":"false") + ")";
		case LUA_TSTRING:	return std::string("V(") + lua_tostring(ls,idx) + ")";
		case LUA_TTABLE:	return "T";
		case LUA_TFUNCTION:	return "F";
		case LUA_TUSERDATA:	return "U";
		case LUA_TLIGHTUSERDATA:return "U";
	}
	return "X";
}

static void logLuaStack( lua_State* ls)
{
	std::ostringstream out;
	int nn = lua_gettop(ls);

	for (int ii=-nn; ii<=-1; ++ii)
	{
		out << ' ' << get_lua_elem( ls, ii);
	}
	LOG_DATA << "[lua stack] " << out.str();
}

void wrap_lua_pushnil( lua_State* ls)					{LOG_DATA2 << "[lua operation] lua_pushnil()"; ::lua_pushnil( ls); logLuaStack( ls);}
void wrap_lua_pushvalue( lua_State* ls, int arg)			{LOG_DATA2 << "[lua operation] lua_pushvalue" << "(" << arg << ")"; ::lua_pushvalue( ls, arg); logLuaStack( ls);}
void wrap_lua_pushboolean( lua_State* ls, bool arg)			{LOG_DATA2 << "[lua operation] lua_pushboolean" << "(" << arg << ")"; ::lua_pushboolean( ls, arg); logLuaStack( ls);}
void wrap_lua_pushinteger( lua_State* ls, int arg)			{LOG_DATA2 << "[lua operation] lua_pushinteger" << "(" << arg << ")"; ::lua_pushinteger( ls, arg); logLuaStack( ls);}
void wrap_lua_pushnumber( lua_State* ls, double arg)			{LOG_DATA2 << "[lua operation] lua_pushnumber" << "(" << arg << ")"; ::lua_pushnumber( ls, arg); logLuaStack( ls);}
void wrap_lua_pushlstring( lua_State* ls, const char* arg, int n)	{LOG_DATA2 << "[lua operation] lua_pushlstring" << "(" << arg << "," << n << ")"; ::lua_pushlstring( ls, arg, n); logLuaStack( ls);}
void wrap_lua_pushstring( lua_State* ls, const char* arg)		{LOG_DATA2 << "[lua operation] lua_pushstring" << "(" << arg << ")"; ::lua_pushstring( ls, arg); logLuaStack( ls);}
void wrap_lua_pop( lua_State* ls, int arg)				{LOG_DATA2 << "[lua operation] lua_pop" << "(" << arg << ")"; ::lua_pop( ls, arg); logLuaStack( ls);}
void wrap_lua_newtable( lua_State* ls)					{LOG_DATA2 << "[lua operation] lua_newtable" << "()"; ::lua_newtable( ls); logLuaStack( ls);}
void wrap_lua_gettable( lua_State* ls, int arg)				{LOG_DATA2 << "[lua operation] lua_gettable" << "(" << arg << ")"; lua_gettable( ls, arg); logLuaStack( ls);}
void wrap_lua_settable( lua_State* ls, int arg)				{LOG_DATA2 << "[lua operation] lua_settable" << "(" << arg << ")"; ::lua_settable( ls, arg); logLuaStack( ls);}
bool wrap_lua_next( lua_State* ls, int arg)				{LOG_DATA2 << "[lua operation] lua_next" << "("  << arg << ")"; int rt = ::lua_next( ls, arg); logLuaStack( ls); return rt;}
const char* wrap_lua_tostring( lua_State* ls, int arg)			{LOG_DATA2 << "[lua operation] lua_tostring" << "("  << arg << ")"; return ::lua_tostring( ls, arg);}
const char* wrap_lua_tolstring( lua_State* ls, int arg, size_t* size)	{LOG_DATA2 << "[lua operation] lua_tolstring" << "("  << arg << ")"; return ::lua_tolstring( ls, arg, size);}
double wrap_lua_tonumber( lua_State* ls, int arg)			{LOG_DATA2 << "[lua operation] lua_tonumber" << "("  << arg << ")"; return ::lua_tonumber( ls, arg);}
bool wrap_lua_toboolean( lua_State* ls, int arg)			{LOG_DATA2 << "[lua operation] lua_toboolean" << "("  << arg << ")"; return ::lua_toboolean( ls, arg);}

#define _wrap_lua_pushnil( ls)		if (m_logtrace) wrap_lua_pushnil( ls); else lua_pushnil(ls);
#define _wrap_lua_pushvalue( ls,a)	if (m_logtrace) wrap_lua_pushvalue(ls,a); else lua_pushvalue(ls,a);
#define _wrap_lua_pushboolean( ls,a)	if (m_logtrace) wrap_lua_pushboolean(ls,a); else lua_pushboolean(ls,a);
#define _wrap_lua_pushinteger( ls,a)	if (m_logtrace) wrap_lua_pushinteger(ls,a); else lua_pushinteger(ls,a);
#define _wrap_lua_pushnumber( ls,a)	if (m_logtrace) wrap_lua_pushnumber(ls,a); else lua_pushnumber(ls,a);
#define _wrap_lua_pushlstring( ls,a,n)	if (m_logtrace) wrap_lua_pushlstring(ls,a,n); else lua_pushlstring(ls,a,n);
#define _wrap_lua_pushstring( ls,a)	if (m_logtrace) wrap_lua_pushstring(ls,a); else lua_pushstring(ls,a);
#define _wrap_lua_pop( ls,a)		if (m_logtrace) wrap_lua_pop(ls,a); else lua_pop(ls,a);
#define _wrap_lua_newtable( ls)		if (m_logtrace) wrap_lua_newtable(ls); else lua_newtable(ls);
#define _wrap_lua_gettable( ls,a)	if (m_logtrace) wrap_lua_gettable(ls,a); else lua_gettable(ls,a);
#define _wrap_lua_settable( ls,a)	if (m_logtrace) wrap_lua_settable(ls,a); else lua_settable(ls,a);
#define _wrap_lua_next( ls,a)		((m_logtrace)?wrap_lua_next(ls,a):lua_next(ls,a))
#define _wrap_lua_tostring( ls,a)	((m_logtrace)?wrap_lua_tostring(ls,a):lua_tostring(ls,a))
#define _wrap_lua_tolstring( ls,a,n)	((m_logtrace)?wrap_lua_tolstring(ls,a,n):lua_tolstring(ls,a,n))
#define _wrap_lua_tonumber( ls,a)	((m_logtrace)?wrap_lua_tonumber(ls,a):lua_tonumber(ls,a))
#define _wrap_lua_toboolean( ls,a)	((m_logtrace)?wrap_lua_toboolean(ls,a):lua_toboolean(ls,a))


LuaTableInputFilter::LuaTableInputFilter( const LuaTableInputFilter& o)
	:TypedInputFilter(o)
	,LuaExceptionHandlerScope(o)
	,m_ls(o.m_ls)
	,m_stk(o.m_stk)
	,m_logtrace(o.m_logtrace)
{
	if (m_stk.size() > 0)
	{
		if (m_stk.size() > 1 || m_stk.back().id != FetchState::Init)
		{
			throw std::runtime_error( "copy of lua input filter not allowed in this state");
		}
	}
}

LuaTableInputFilter::LuaTableInputFilter( lua_State* ls)
	:TypedInputFilter("lua")
	,LuaExceptionHandlerScope(ls)
	,m_ls(ls)
	,m_logtrace(_Wolframe::log::LogBackend::instance().minLogLevel() == _Wolframe::log::LogLevel::LOGLEVEL_DATA2)
{
	setFlags( langbind::FilterBase::PropagateNoAttr);

	FetchState fs( FetchState::Init, 0/*name*/, 0/*std::strlen(name)*/);
	m_stk.push_back( fs);
}

void LuaTableInputFilter::FetchState::getTagElement( types::VariantConst& element) const
{
	element.init( tagname, tagnamesize);
}

bool LuaTableInputFilter::getLuaStackValue( int idx, types::VariantConst& element)
{
	const char* lstr;
	std::size_t lstrlen = 0;
	switch (lua_type( m_ls, idx))
	{
		case LUA_TNIL:
			return false;

		case LUA_TNUMBER:
		{
			double num = (double)_wrap_lua_tonumber(m_ls, idx);
			double flo = std::floor( num);
			if (num - flo <= std::numeric_limits<double>::epsilon())
			{
				element = boost::numeric_cast<types::Variant::Data::Int>(num);
			}
			else
			{
				element = num;
			}
			return true;
		}
		case LUA_TBOOLEAN:
			element = (bool)_wrap_lua_toboolean(m_ls, idx);
			return true;

		case LUA_TSTRING:
			lstr = (const char*)_wrap_lua_tolstring( m_ls, idx, &lstrlen);
			element.init( lstr, lstrlen);
			return true;

		case LUA_TUSERDATA:
		{
			types::BigNumber* bn = LuaObject<types::BigNumber>::get( m_ls, idx);
			if (bn)
			{
				element.init( *bn);
				return true;
			}
			types::DateTime* dt = LuaObject<types::DateTime>::get( m_ls, idx);
			if (dt)
			{
				element.init( *dt);
				return true;
			}
			types::CustomDataValueR* cv = LuaObject<types::CustomDataValueR>::get( m_ls, idx);
			if (cv)
			{
				element.init( **cv);
				return true;
			}
		}
		default:
		{
			const char* errelemtype = lua_typename( m_ls, lua_type( m_ls, idx));
			std::ostringstream msg;
			msg << "element type '" << (errelemtype?errelemtype:"unknown") << "' not expected for atomic value in filter";
			setState( InputFilter::Error, msg.str().c_str());
			return false;
		}
	}
}

std::string LuaTableInputFilter::stackDump( const std::vector<FetchState>& stk)
{
	std::ostringstream out;
	std::vector<LuaTableInputFilter::FetchState>::const_iterator si = stk.begin(), se = stk.end();
	for (; si != se; ++si)
	{
		if (si != stk.begin()) out << ",";
		out << FetchState::idName( si->id);
		if (si->tagname) out << "(" << si->tagname << ")";
		if (si->idx > 0)
		{
			out << ":" << si->idx;
		}
	}
	return out.str();
}

static bool luatable_isarray( lua_State* ls, int idx)
{
	std::size_t len = lua_rawlen( ls, idx);
	lua_pushinteger( ls, len);
	lua_gettable( ls, idx-1);
	bool rt = !lua_isnil( ls, -1);
	lua_pop( ls, 1);
	return rt;
}

bool LuaTableInputFilter::checkTableArrayIndex( int idx, bool topLevel)
{
	if (lua_type( m_ls, -2) != LUA_TNUMBER || lua_tointeger( m_ls, -2) != idx)
	{
		_wrap_lua_pop( m_ls, 1);
		setState( InputFilter::Error, "mixed content in lua table: array (table with number indices) with map (associative)");

		if (!topLevel)
		{
			//... discard array element too if not top level
			_wrap_lua_pop( m_ls, 2);
		}
		return false;
	}
	return true;
}

bool LuaTableInputFilter::createTableElemState( FetchState& st)
{
	std::size_t elemnamesize;
	const char* elemname = _wrap_lua_tolstring( m_ls, -2, &elemnamesize);
	st = FetchState( FetchState::TableElemValue, elemname, elemnamesize);

	if (lua_type( m_ls, -1) == LUA_TTABLE && luatable_isarray( m_ls, -1))
	{
		_wrap_lua_pushnil( m_ls);
		if (!_wrap_lua_next( m_ls, -2))
		{
			_wrap_lua_pop( m_ls, 2); //... pop structure key and value
			return false;
		}
		if (!checkTableArrayIndex( 1, false))
		{
			_wrap_lua_pop( m_ls, 2); //... pop table element key and value
			return false;
		}
		st.idx = 1; //... define as array
	}
	return true;
}

bool LuaTableInputFilter::fetchFirstTableElem()
{
	_wrap_lua_pushnil( m_ls);
	if (!_wrap_lua_next( m_ls, -2))
	{
		return false;
	}
	switch (lua_type( m_ls, -2))
	{
		case LUA_TNUMBER:
		{
			if (luatable_isarray( m_ls, -3))
			{
				if (!checkTableArrayIndex( 1, true))
				{
					return false;
				}
				m_stk.push_back( FetchState( FetchState::TableElemValue, 0, 0));
				//... top level array (no naming tag defined)
				m_stk.back().idx = 1;
				return true;
			}
			/*no break here!*/
		}
		case LUA_TSTRING:
		{
			FetchState st;
			if (!createTableElemState(st)) return false;
			m_stk.push_back( st);
			return true;
		}
		default:
		{
			setState( InputFilter::Error, "cannot treat table as a struct nor an array");
			_wrap_lua_pop( m_ls, 2);
			return false;
		}
	}
}

bool LuaTableInputFilter::fetchNextTableElem()
{
	_wrap_lua_pop( m_ls, 1);
	if (!_wrap_lua_next( m_ls, -2))
	{
		if (m_stk.back().isArray() && !m_stk.back().isTopLevel())
		{
			m_stk.back().idx = 0;
			//... next element is a single structure element
			return fetchNextTableElem();
		}
		m_stk.pop_back();
		return false;
	}
	if (m_stk.back().isArray())
	{
		// Check the index of the array:
		++m_stk.back().idx;
		if (!checkTableArrayIndex( m_stk.back().idx, m_stk.back().isTopLevel()))
		{
			m_stk.pop_back();
			return false;
		}
		m_stk.back().id = FetchState::TableElemValue;
	}
	else
	{
		FetchState st;
		if (!createTableElemState( st)) return false;
		m_stk.back() = st;
	}
	return true;
}

bool LuaTableInputFilter::getNext( ElementType& type, types::VariantConst& element)
{
	if (!lua_checkstack( m_ls, 16))
	{
		setState( InputFilter::Error, "lua stack overflow");
		return false;
	}
	setState( InputFilter::Open);
	for (;;)
	{
		if (m_stk.size() == 0) return false;

		switch (m_stk.back().id)
		{
			case FetchState::Init:
				m_stk.back().id = FetchState::Value;
				continue;

			case FetchState::Value:
				if (lua_istable( m_ls, -1))
				{
					m_stk.back().id = FetchState::TableCloseElem;
					if (fetchFirstTableElem())
					{
						if (m_stk.back().tagname)
						{
							element = m_stk.back().tagname;
							if (m_stk.back().isArray())
							{
								type = InputFilter::OpenTagArray;
							}
							else
							{
								type = InputFilter::OpenTag;
							}
							return true;
						}
					}
					else
					{
						if (state() != InputFilter::Open)
						{
							return false;
						}
					}
					continue;
				}
				else if (lua_isnil( m_ls, -1))
				{
					m_stk.back().id = FetchState::Done;
					continue;
				}
				else
				{
					m_stk.back().id = FetchState::Done;
					getLuaStackValue( -1, element);
					type = InputFilter::Value;
					return true;
				}

			case FetchState::TableElemValue:
				//... get element name
				if (lua_istable( m_ls, -1))
				{
					m_stk.back().id = FetchState::TableCloseElem;
					if (fetchFirstTableElem())
					{
						if (m_stk.back().tagname)
						{
							element = m_stk.back().tagname;
							if (m_stk.back().isArray())
							{
								type = InputFilter::OpenTagArray;
							}
							else
							{
								type = InputFilter::OpenTag;
							}
							return true;
						}
					}
					else
					{
						if (state() != InputFilter::Open)
						{
							return false;
						}
					}
					continue;
				}
				else if (lua_isnil( m_ls, -1))
				{
					m_stk.back().id = FetchState::TableCloseElem;
				}
				else
				{
					m_stk.back().id = FetchState::TableCloseElem;
					getLuaStackValue( -1, element);
					type = InputFilter::Value;
					return true;
				}

			case FetchState::TableCloseElem:
				m_stk.back().id = FetchState::TableNextElem;
				type = CloseTag;
				element.init();
				return true;

			case FetchState::TableNextElem:
				if (fetchNextTableElem())
				{
					if (m_stk.back().isArray())
					{
						if (!m_stk.back().tagname) continue;

						type = InputFilter::OpenTagArray;
						element = m_stk.back().tagname;
						return true;
					}
					else
					{
						getLuaStackValue( -2, element);
						type = InputFilter::OpenTag;
						return true;
					}
				}
				else
				{
					if (state() != InputFilter::Open)
					{
						return false;
					}
					continue;
				}
				continue;

			case FetchState::Done:
				m_stk.pop_back();
				type = CloseTag;
				element.init();
				return true;
		}
	}
	setState( InputFilter::Error, "illegal state in lua filter get next");
	return false;
}


LuaTableOutputFilter::LuaTableOutputFilter( lua_State* ls)
	:TypedOutputFilter("lua")
	,LuaExceptionHandlerScope(ls)
	,m_ls(ls)
	,m_type(OpenTag)
	,m_logtrace(_Wolframe::log::LogBackend::instance().minLogLevel() == _Wolframe::log::LogLevel::LOGLEVEL_DATA2){}

LuaTableOutputFilter::LuaTableOutputFilter( const LuaTableOutputFilter& o)
	:TypedOutputFilter(o)
	,LuaExceptionHandlerScope(o)
	,m_ls(o.m_ls)
	,m_type(o.m_type)
	,m_logtrace(o.m_logtrace)
	,m_statestk(o.m_statestk)
{
	if (m_statestk.size() > 0) throw std::runtime_error( "copy of lua output filter not allowed in this state");
}

bool LuaTableOutputFilter::pushValue( const types::VariantConst& element)
{
	switch (element.type())
	{
		case types::Variant::Custom:
			LuaObject<types::CustomDataValueR>::push_luastack( m_ls, types::CustomDataValueR( element.customref()->copy()));
			return true;
		case types::Variant::Null:
			_wrap_lua_pushnil( m_ls);
			return true;
		case types::Variant::Bool:
			_wrap_lua_pushboolean( m_ls, element.tobool());
			return true;
		case types::Variant::Double:
			_wrap_lua_pushnumber( m_ls, (lua_Number)element.todouble());
			return true;
		case types::Variant::Int:
			_wrap_lua_pushinteger( m_ls, (lua_Integer)element.toint());
			return true;
		case types::Variant::UInt:
			_wrap_lua_pushinteger( m_ls, (lua_Integer)element.touint());
			return true;
		case types::Variant::String:
			_wrap_lua_pushlstring( m_ls, element.charptr(), element.charsize());
			return true;
		case types::Variant::Timestamp:
			LuaObject<types::DateTime>::push_luastack( m_ls, types::DateTime( element.totimestamp()));
			return true;
		case types::Variant::BigNumber:
			LuaObject<types::BigNumber>::push_luastack( m_ls, types::BigNumber( *element.bignumref()));
			return true;
	}
	setState( OutputFilter::Error, "illegal value type of element");
	return false;
}

bool LuaTableOutputFilter::openTag( const types::VariantConst& element, bool array)
{
	if (!pushValue( element)) return false;				///... LUA STK: t k   (t=Table,k=Key)
	_wrap_lua_pushvalue( m_ls,-1);					///... LUA STK: t k k
	_wrap_lua_gettable( m_ls, -3);					///... LUA STK: t k t[k]
	if (lua_isnil( m_ls, -1))
	{
		///... element with this key not yet defined. so we define it
		_wrap_lua_pop( m_ls, 1);				///... LUA STK: t k
		_wrap_lua_newtable( m_ls);				///... LUA STK: t k NEWTABLE
		if (array)
		{
			_wrap_lua_pushinteger( m_ls, 1);		///... LUA STK: t k NEWTABLE 1
			_wrap_lua_newtable( m_ls);					///... LUA STK: t k ar 2 NEWTABLE
			m_statestk.push_back( Vector);
		}
		else
		{
			m_statestk.push_back( Struct);
		}
		return true;
	}
	// check for t[k] beeing table and t[k][#t[k]] exists -> t[k] is an array:
	if (lua_istable( m_ls, -1))					///... LUA STK: t k t[k]
	{
		std::size_t len = lua_rawlen( m_ls, -1);
		_wrap_lua_pushinteger( m_ls, len);			///... LUA STK: t k t[k] #t[k]
		_wrap_lua_gettable( m_ls, -2);				///... LUA STK: t k t[k] t[k][#t[k]]
		if (!lua_isnil( m_ls, -1))
		{
			///... the table is an array
			_wrap_lua_pop( m_ls, 1);			///... LUA STK: t k t[k]
			_wrap_lua_pushinteger( m_ls, len+1);		///... LUA STK: t k ar len+1
			_wrap_lua_newtable( m_ls);			///... LUA STK: t k ar len+1 NEWTABLE
			m_statestk.push_back( Vector);
			return true;
		}
		else
		{
			_wrap_lua_pop( m_ls, 1);			///... LUA STK: t k t[k]
		}
	}
	///... the element is a structure or atomic element already defined. we create an array
	_wrap_lua_pop( m_ls, 1);					///... LUA STK: t k
	_wrap_lua_newtable( m_ls);					///... LUA STK: t k ar
	_wrap_lua_pushinteger( m_ls, 1);				///... LUA STK: t k ar 1
	_wrap_lua_pushvalue( m_ls, -3);					///... LUA STK: t k ar 1 k
	_wrap_lua_gettable( m_ls, -5);					///... LUA STK: t k ar 1 t[k]
	_wrap_lua_settable( m_ls, -3);					///... LUA STK: t k ar             (ar[1]=t[k])
	_wrap_lua_pushinteger( m_ls, 2);				///... LUA STK: t k ar 2
	_wrap_lua_newtable( m_ls);					///... LUA STK: t k ar 2 NEWTABLE
	m_statestk.push_back( Vector);
	return true;
}

bool LuaTableOutputFilter::closeTag()
{
	if (m_statestk.size() > 0)
	{
		if (m_statestk.size() == 1)
		{
			m_statestk.pop_back();
		}
		else if (m_statestk.back().m_contentType == Vector)
		{
			m_statestk.pop_back();				//... LUA STK: t k ar i v
			_wrap_lua_settable( m_ls, -3);			//... LUA STK: t k ar		(ar[i] = v)
			_wrap_lua_settable( m_ls, -3);			//... LUA STK: t		(t[k] = ar)
		}
		else
		{
			m_statestk.pop_back();				//... LUA STK: t k v
			_wrap_lua_settable( m_ls, -3);			//... LUA STK: t		(t[k] = v)
		}
		if (m_statestk.size())
		{
			++m_statestk.back().m_idx;
		}
		return true;
	}
	else
	{
		setState( OutputFilter::Error, "tags not balanced");
		return false;
	}
}

bool LuaTableOutputFilter::closeAttribute( const types::VariantConst& element)
{
	if (!lua_istable( m_ls, -1))
	{
		setState( OutputFilter::Error, "illegal state (internal): closeAttribute called on non table");
		return false;
	}
	_wrap_lua_pushnil( m_ls);					//... LUA STK: t a t nil
	if (_wrap_lua_next( m_ls, -2))
	{
		// ... non empty table, we create an element with an empty key
		_wrap_lua_pop( m_ls, 2);
		return openTag( types::VariantConst("")) && closeAttribute( element) && closeTag();
	}
	else
	{
		// ... table empty. replace it by value 'element'
		_wrap_lua_pop( m_ls, 1);				//... LUA STK: t a
		return pushValue( element);				//... LUA STK: t a v
	}
}

bool LuaTableOutputFilter::print( ElementType type, const types::VariantConst& element)
{
	LOG_DATA << "[lua table] push element " << langbind::InputFilter::elementTypeName( type) << " '" << utils::getLogString( element) << "'";
	if (!lua_checkstack( m_ls, 16))
	{
		setState( OutputFilter::Error, "lua stack overflow");
		return false;
	}
	if (m_statestk.size() == 0)
	{
		if (state() == OutputFilter::Start)
		{
			setState( OutputFilter::Open);
			m_statestk.push_back( Struct);
			_wrap_lua_newtable( m_ls);
		}
		else
		{
			setState( OutputFilter::Error, "tags not balanced in input: print called after final close");
			return false;
		}
	}
	switch (type)
	{
		case OpenTag:
		case OpenTagArray:
			switch (m_type)
			{
				case Attribute:
				case Value:
					setState( OutputFilter::Error, "unexpected open tag");
					return false;
				case OpenTag:
				case OpenTagArray:
				case CloseTag:
					m_type = type;
					return openTag( element, type==OpenTagArray);
			}
			break;

		case Attribute:
			switch (m_type)
			{
				case Attribute:
				case Value:
				case CloseTag:
					setState( OutputFilter::Error, "unexpected attribute");
					return false;
				case OpenTag:
				case OpenTagArray:
					m_type = type;
					return openTag( element, type==OpenTagArray);
			}
			break;

		case Value:
			if (m_statestk.size())
			{
				++m_statestk.back().m_idx;
			}
			switch (m_type)
			{
				case Attribute:
					m_type = OpenTag;
					//... back to open tag
					return closeAttribute( element) && closeTag();
				case Value:
					setState( OutputFilter::Error, "got two subsequent values without enclosing tag (without vector index or table element name)");
					return false;

				case OpenTag:
				case OpenTagArray:
				case CloseTag:
					m_type = type;
					return closeAttribute( element);
			}

		case CloseTag:
			switch (m_type)
			{
				case Attribute:
					setState( OutputFilter::Error, "output filter of attribute value missed");
					return false;
				case OpenTagArray:
				case OpenTag:
					m_type = type;
					if (!m_statestk.size() || m_statestk.back().m_idx == 0)
					{
						if (m_statestk.back().m_contentType == Vector)
						{
							_wrap_lua_pop( m_ls, 4);
						}
						else
						{
							_wrap_lua_pop( m_ls, 2);
						}
						return true;
					}
					/*no break here!*/
				case Value:
				case CloseTag:
					m_type = type;
					return closeTag();
			}
	}
	setState( OutputFilter::Error, "illegal state");
	return false;
}



