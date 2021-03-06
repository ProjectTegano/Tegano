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
///\file luaFunctionProgramType.cpp
///\brief Implementation of the function to create a form function program type object for lua scripts
#include "luaFunctionProgramType.hpp"
#include "langbind/formFunction.hpp"
#include "luaScriptContext.hpp"
#include "processor/procProviderInterface.hpp"
#include "processor/execContext.hpp"
#include "processor/programLibrary.hpp"
#include "luaObjects.hpp"
#include "logger/logger-v1.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

using namespace _Wolframe;
using namespace _Wolframe::langbind;

namespace {
class LuaFormFunctionClosure
	:public FormFunctionClosure
{
public:
	LuaFormFunctionClosure( const LuaScriptInstanceR& interp_, const std::string& name_)
		:m_interp(interp_),m_name(name_),m_firstcall(false)
	{}

	virtual ~LuaFormFunctionClosure()
	{}

	virtual bool call()
	{
		if (m_firstcall)
		{
			lua_getglobal( m_interp->thread(), m_name.c_str());
			if (!m_arg.get())
			{
				LOG_ERROR << "lua function got no valid argument";
			}
			m_interp->pushObject( m_arg);
			m_firstcall = false;
		}
		// call the lua form function (subsequently until termination)
		int rt = lua_resume( m_interp->thread(), NULL, 1);
		if (rt == LUA_YIELD) return false;
		if (rt != 0)
		{
			LOG_ERROR << "error calling lua form function '" << m_name.c_str() << "':" << m_interp->luaErrorMessage( m_interp->thread());
			throw std::runtime_error( m_interp->luaUserErrorMessage( m_interp->thread()));
		}
		m_result = m_interp->getObject( -1);
		if (!m_result.get())
		{
			throw std::runtime_error( "lua function called returned no result or nil (structure expected)");
		}
		return true;
	}

	///\remark Flags ignored because lua has no strict typing does not validate input parameter structure on its own
	virtual void init( proc::ExecContext* ctx, const TypedInputFilterR& arg, serialize::ValidationFlags::Enum)
	{
		m_interp->init( ctx);
		m_arg = arg;
		if (m_arg->flag( TypedInputFilter::PropagateNoArray))
		{
			LOG_WARNING << "Calling Lua function '" << m_name << "' without array element info";
		}
		m_firstcall = true;
	}

	virtual TypedInputFilterR result() const
	{
		return m_result;
	}

private:
	LuaScriptInstanceR m_interp;
	std::string m_name;
	TypedInputFilterR m_arg;
	TypedInputFilterR m_result;
	bool m_firstcall;
};

class LuaFormFunction
	:public FormFunction
{
public:
	LuaFormFunction( const LuaScriptContext* context_, const std::string& name_)
		:m_context(context_),m_name(name_){}

	virtual ~LuaFormFunction(){}

	virtual FormFunctionClosure* createClosure() const
	{
		LuaScriptInstanceR interp( m_context->funcmap.createLuaScriptInstance( m_name));
		return new LuaFormFunctionClosure( interp, m_name);
	}

private:
	const LuaScriptContext* m_context;
	std::string m_name;
};
}//anonymous namespace


bool LuaProgramType::is_mine( const std::string& filename) const
{
	boost::filesystem::path p( filename);
	return p.extension().string() == ".lua";
}

void LuaProgramType::loadProgram( proc::ProgramLibrary& library, const std::string& filename)
{
	std::vector<std::string> funcs = m_context.loadProgram( filename);
	std::vector<std::string>::const_iterator fi = funcs.begin(), fe = funcs.end();
	for (; fi != fe; ++fi)
	{
		FormFunctionR ff( new LuaFormFunction( &m_context, *fi));
		library.defineFormFunction( *fi, ff);
	}
}



