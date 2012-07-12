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
///\file langbind/luaTypeTraits.hpp
///\brief Type Traits for embedding types in lua binding
#ifndef _Wolframe_LANGBIND_LUA_TYPE_TRAITS_HPP_INCLUDED
#define _Wolframe_LANGBIND_LUA_TYPE_TRAITS_HPP_INCLUDED
#include "langbind/luaException.hpp"
#include <string>
#include <stdexcept>
#include <cstring>
#include <boost/integer_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/remove_const.hpp>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

namespace _Wolframe {
namespace langbind {

///\brief Operators and Methods checked in this module
template<typename TYPE>
struct Interface
{
	TYPE operator + ( const TYPE&) const;
	TYPE operator - ( const TYPE&) const;
	TYPE operator / ( const TYPE&) const;
	TYPE operator * ( const TYPE&) const;
	TYPE pow( unsigned int) const;
	TYPE operator - () const;
	unsigned int len() const;
	std::string tostring() const;

	bool set( const char* key, const std::string&);
	bool set( const char* key, const char*);
	bool set( const std::string&);
	bool set( const char*);
	bool get( const char* key, std::string&) const;

	bool operator <( const TYPE&) const;
	bool operator <=( const TYPE&) const;
	bool operator ==( const TYPE&) const;
};


template<typename T>
struct has_operator_tostring
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<const std::string& (T::*)() const> struct tester_const_member_signature;
	template<std::string (T::*)() const> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::tostring >*);
	template<typename U>
	static small_type has_matching_member(tester_const_member_signature<&U::tostring >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_len
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<std::size_t (T::*)()> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::len >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_neg
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<T (T::*)()> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::operator- >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_add
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<T (T::*)( const T&) const> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::operator+ >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_sub
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<T (T::*)( const T&) const> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::operator- >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_mul
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<T (T::*)( const T&) const> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::operator* >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_div
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<T (T::*)( const T&) const> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::operator/ >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_le
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<bool (T::*)( const T&)> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::operator<= >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_lt
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<bool (T::*)( const T&)> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::operator< >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_eq
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<bool (T::*)( const T&)> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::operator== >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template <class ArithmeticType, class MetaTypeInfo>
struct LuaArithmeticType :public ArithmeticType
{
	LuaArithmeticType( const std::string& str)
		:ArithmeticType(str){}
	LuaArithmeticType( const ArithmeticType& o)
		:ArithmeticType(o){}
	LuaArithmeticType(){}

	static void createMetatable( lua_State* ls)
	{
		luaL_newmetatable( ls, MetaTypeInfo::metatableName());

		lua_pushliteral( ls, "__gc");
		lua_pushcfunction( ls, &destroy);
		lua_rawset( ls, -3);
		lua_pop( ls, 1);
	}

	void* operator new (std::size_t num_bytes, lua_State* ls) throw (std::bad_alloc)
	{
		void* rt = lua_newuserdata( ls, num_bytes);
		if (rt == 0) throw std::bad_alloc();
		luaL_getmetatable( ls, MetaTypeInfo::metatableName());
		lua_setmetatable( ls, -2);
		return rt;
	}

	//\brief does nothing because the LUA garbage collector does the job.
	//\warning CAUTION: DO NOT CALL THIS FUNCTION ! DOES NOT WORK ON MSVC 9.0. (The compiler links with the std delete) (just avoids C4291 warning)
	void operator delete (void *, lua_State*) {}

	static int destroy( lua_State* ls)
	{
		LuaArithmeticType *THIS = (LuaArithmeticType*)lua_touserdata( ls, 1);
		if (THIS) THIS->~LuaArithmeticType();
		return 0;
	}

	static void push_luastack( lua_State* ls, const ArithmeticType& o)
	{
		try
		{
			(void*)new (ls) LuaArithmeticType( o);
		}
		catch (const std::exception&)
		{
			luaL_error( ls, "memory allocation error in lua context");
		}
	}

	static ArithmeticType* get( lua_State* ls, int index)
	{
		ArithmeticType* rt = (ArithmeticType*) luaL_testudata( ls, index, MetaTypeInfo::metatableName());
		if (!rt)
		{
			const char* arg = lua_tostring( ls, index);
			if (arg == 0) throw std::runtime_error( "cannot convert argument to arithmetic type");
			LuaExceptionHandlerScope escope(ls);
			{
				(void)new (ls) LuaArithmeticType( std::string(arg));
			}
			rt = (ArithmeticType*) luaL_checkudata( ls, -1, MetaTypeInfo::metatableName());
		}
		return rt;
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaBinOperatorAdd
{
	typedef LuaBinOperatorAdd This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			ObjectType* operand = ThisOperand::get( ls, 2);
			ThisOperand::push_luastack( ls, self->operator+( *operand));
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__add", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaBinOperatorSub
{
	typedef LuaBinOperatorSub This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			ObjectType* operand = ThisOperand::get( ls, 2);
			ThisOperand::push_luastack( ls, self->operator-( *operand));
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__sub", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaBinOperatorMul
{
	typedef LuaBinOperatorMul This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			ObjectType* operand = ThisOperand::get( ls, 2);
			ThisOperand::push_luastack( ls, self->operator*( *operand));
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__mul", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaBinOperatorDiv
{
	typedef LuaBinOperatorDiv This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			ObjectType* operand = ThisOperand::get( ls, 2);
			ThisOperand::push_luastack( ls, self->operator/( *operand));
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__div", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaOperatorNeg
{
	typedef LuaOperatorNeg This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			ThisOperand::push_luastack( ls, self->operator-());
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__neg", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaOperatorToString
{
	typedef LuaOperatorToString This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			std::string content = self->tostring();
			lua_pushlstring( ls, content.c_str(), content.size());
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__tostring", ls);
	}
};


template <class ObjectType, class MetaTypeInfo>
struct LuaCmpOperatorEqual
{
	typedef LuaCmpOperatorEqual This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			ObjectType* operand = ThisOperand::get( ls, 2);
			lua_pushboolean( ls, self->operator==( *operand));
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__eq", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaCmpOperatorLessEqual
{
	typedef LuaCmpOperatorLessEqual This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			ObjectType* operand = ThisOperand::get( ls, 2);
			lua_pushboolean( ls, self->operator<=( *operand));
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__le", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaCmpOperatorLessThan
{
	typedef LuaCmpOperatorLessThan This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			ObjectType* operand = ThisOperand::get( ls, 2);
			lua_pushboolean( ls, self->operator<( *operand));
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__lt", ls);
	}
};


template <typename T, typename M>
typename boost::enable_if_c<
	has_operator_eq<T>::value
	,void>::type defineOperator_eq( lua_State* ls)
{
	lua_pushliteral( ls, "__eq");
	lua_CFunction f = &LuaCmpOperatorEqual<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	! has_operator_eq<T>::value
	,void>::type defineOperator_eq( lua_State*)
{}

template <typename T, typename M>
typename boost::enable_if_c<
	has_operator_le<T>::value
	,void>::type defineOperator_le( lua_State* ls)
{
	lua_pushliteral( ls, "__le");
	lua_CFunction f = &LuaCmpOperatorLessEqual<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	! has_operator_le<T>::value
	,void>::type defineOperator_le( lua_State*)
{}

template <typename T, typename M>
typename boost::enable_if_c<
	has_operator_lt<T>::value
	,void>::type defineOperator_lt( lua_State* ls)
{
	lua_pushliteral( ls, "__lt");
	lua_CFunction f = &LuaCmpOperatorLessThan<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	! has_operator_lt<T>::value
	,void>::type defineOperator_lt( lua_State*)
{}

template <typename T, typename M>
typename boost::enable_if_c<
	has_operator_tostring<T>::value
	,void>::type defineOperator_tostring( lua_State* ls)
{
	lua_pushliteral( ls, "__tostring");
	lua_CFunction f = &LuaOperatorToString<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	! has_operator_tostring<T>::value
	,void>::type defineOperator_tostring( lua_State*)
{}

template <typename T, typename M>
typename boost::enable_if_c<
	has_operator_neg<T>::value
	,void>::type defineOperator_neg( lua_State* ls)
{
	lua_pushliteral( ls, "__unm");
	lua_CFunction f = &LuaOperatorNeg<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	! has_operator_neg<T>::value
	,void>::type defineOperator_neg( lua_State*)
{}

template <typename T, typename M>
typename boost::enable_if_c<
	has_operator_add<T>::value
	,void>::type defineOperator_add( lua_State* ls)
{
	lua_pushliteral( ls, "__add");
	lua_CFunction f = &LuaBinOperatorAdd<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	! has_operator_add<T>::value
	,void>::type defineOperator_add( lua_State*)
{}

template <typename T, typename M>
typename boost::enable_if_c<
	has_operator_sub<T>::value
	,void>::type defineOperator_sub( lua_State* ls)
{
	lua_pushliteral( ls, "__sub");
	lua_CFunction f = &LuaBinOperatorSub<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	! has_operator_sub<T>::value
	,void>::type defineOperator_sub( lua_State*)
{}

template <typename T, typename M>
typename boost::enable_if_c<
	has_operator_div<T>::value
	,void>::type defineOperator_div( lua_State* ls)
{
	lua_pushliteral( ls, "__div");
	lua_CFunction f = &LuaBinOperatorDiv<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	! has_operator_div<T>::value
	,void>::type defineOperator_div( lua_State*)
{}

template <typename T, typename M>
typename boost::enable_if_c<
	has_operator_mul<T>::value
	,void>::type defineOperator_mul( lua_State* ls)
{
	lua_pushliteral( ls, "__mul");
	lua_CFunction f = &LuaBinOperatorMul<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	! has_operator_mul<T>::value
	,void>::type defineOperator_mul( lua_State*)
{}

}} //namespace
#endif

