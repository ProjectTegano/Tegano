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
/// \file luafilter.hpp
/// \brief Interface for a lua table as filter

#ifndef _Wolframe_LUA_FILTER_HPP_INCLUDED
#define _Wolframe_LUA_FILTER_HPP_INCLUDED
#include "luaException.hpp"
#include "filter/typedfilter.hpp"
#include "types/variant.hpp"
#include <vector>
#include <stdexcept>

#if WITH_LUA
extern "C" {
	#include "lua.h"
}
#else
#error Lua support not enabled
#endif

namespace _Wolframe {
namespace langbind {

/// \class LuaTableInputFilter
/// \brief Lua table as typed input filter
class LuaTableInputFilter :public TypedInputFilter, public LuaExceptionHandlerScope
{
public:
	/// \brief Constructor
	/// \remark Expects that the lua stack is not modified by anyone but this class in the lifetime after the first call of LuaInputFilter::getNext(ElementType&,types::VariantConst&)
	explicit LuaTableInputFilter( lua_State* ls);

	/// \brief Copy constructor
	/// \param[in] o lua table input filter to copy
	LuaTableInputFilter( const LuaTableInputFilter& o);

	/// \brief Destructor
	/// \remark Leaves the iterated table as top element (-1) on the lua stack
	virtual ~LuaTableInputFilter(){}

	/// \brief Get a self copy
	/// \return allocated pointer to copy of this
	virtual TypedInputFilter* copy() const		{return new LuaTableInputFilter(*this);}

	/// \brief Implementation of TypedInputFilter::getNext(ElementType&,types::VariantConst&)
	/// \remark Expects the table to iterate as top element (-1) on the lua stack when called the first time
	virtual bool getNext( ElementType& type, types::VariantConst& element);

private:
	/// \class FetchState
	/// \brief Internal state of the iterator
	struct FetchState
	{
		enum Id
		{
			Init,			///< init state (Value first time)
			Value,			///< to fetch the next toplevel value
			TableElemValue,		///< handle the element value
			TableCloseElem,		///< close current table element
			TableNextElem,		///< fetch next table element
			Done			///< job done, discard state
		};
		static const char* idName( Id i)
		{
			static const char* ar[] = {"Init","Value","TableElemValue","TableCloseElem","TableNextElem","Done"};
			return ar[ (int)i];
		}

		FetchState()								:id(Init),tagname(0),tagnamesize(0),idx(0){}
		FetchState( const FetchState& o)					:id(o.id),tagname(o.tagname),tagnamesize(o.tagnamesize),idx(o.idx){}
		FetchState( Id id_, const char* tagname_, std::size_t tagnamesize_)	:id(id_),tagname(tagname_),tagnamesize(tagnamesize_),idx(0){}

		Id id;				///< state identifier
		const char* tagname;		///< caller tag, used enclosing tag by arrays
		std::size_t tagnamesize;	///< size of tag
		int idx;			///< array index

		void getTagElement( types::VariantConst& element) const;
		bool isArray() const
		{
			return idx;
		}
		bool isTopLevel() const
		{
			return !tagname;
		}
	};

private:
	/// \brief Fetch the element with index 'idx' as atomic value from lua stack
	bool getLuaStackValue( int idx, types::VariantConst& e);
	/// \brief Opens a new table iterator on a Lua array or Lua table
	bool fetchFirstTableElem();
	/// \brief Fetches the next element of the currently iterated lua table
	bool fetchNextTableElem();
	/// \brief Helper function to create the context of a table element
	bool createTableElemState( FetchState& st);
	/// \brief Helper function to check the index of an array element
	bool checkTableArrayIndex( int idx, bool topLevel);
	/// \brief Get dump of the iterator stack for debugging purposes (logging)
	static std::string stackDump( const std::vector<FetchState>& stk);

private:
	lua_State* m_ls;			///< lua state
	std::vector<FetchState> m_stk;		///< stack of iterator states
	bool m_logtrace;			///< true, if logging of stack trace is enabled
};


/// \class LuaTableOutputFilter
/// \brief Lua table as typed output filter
class LuaTableOutputFilter :public TypedOutputFilter, public LuaExceptionHandlerScope
{
public:
	/// \brief Constructor
	/// \remark Expects that the lua stack is not modified by anyone but this class in the lifetime after the first call of LuaTableOutputFilter::print(ElementType,const types::VariantConst&)
	explicit LuaTableOutputFilter( lua_State* ls);

	/// \brief Copy constructor
	/// \param[in] o lua output filter to copy
	LuaTableOutputFilter( const LuaTableOutputFilter& o);

	/// \brief Destructor
	virtual ~LuaTableOutputFilter(){}

	/// \brief Get a self copy
	/// \return allocated pointer to copy of this
	virtual TypedOutputFilter* copy() const		{return new LuaTableOutputFilter(*this);}

	/// \brief Implementation of TypedOutputFilter::print(ElementType,const types::VariantConst&)
	virtual bool print( ElementType type, const types::VariantConst& element);

private:
	bool pushValue( const types::VariantConst& element);
	bool openTag( const types::VariantConst& element, bool array=false);
	bool closeTag();
	bool closeAttribute( const types::VariantConst& element);

private:
	enum ContentType
	{
		Atomic,
		Struct,
		Vector
	};
	struct PrintState
	{
		ContentType m_contentType;
		int m_idx;

		PrintState()
			:m_contentType(Vector),m_idx(0){}
		PrintState( ContentType contentType_, int idx_=0)
			:m_contentType(contentType_),m_idx(idx_){}
		PrintState( const PrintState& o)
			:m_contentType(o.m_contentType),m_idx(o.m_idx){}
	};
	lua_State* m_ls;				///< lua state for building the result
	ElementType m_type;				///< last element type printed
	bool m_logtrace;				///< true, if logging of stack trace is enabled
	std::vector<PrintState> m_statestk;		///< state stack that tells what substructures are build
};

}}//namespace
#endif

