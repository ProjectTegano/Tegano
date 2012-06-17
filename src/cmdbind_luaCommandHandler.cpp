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
#include "cmdbind/luaCommandHandler.hpp"
#include "langbind/luaDebug.hpp"
#include "langbind/appObjects.hpp"
#include "langbind/luaObjects.hpp"
#include "langbind/appGlobalContext.hpp"
#include "logger-v1.hpp"
#include <stdexcept>
#include <cstddef>
#include <boost/lexical_cast.hpp>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

using namespace _Wolframe;
using namespace langbind;
using namespace cmdbind;

LuaCommandHandler::CallResult LuaCommandHandler::call( const char*& errorCode)
{
	int rt = 0;
	errorCode = 0;
	int nargs = 0;

	if (!m_interp.get())
	{
		try
		{
			GlobalContext* gc = getGlobalContext();
			LuaScriptInstanceR sc;
			if (!gc->getLuaScriptInstance( m_name, m_interp))
			{
				LOG_ERROR << "unknown lua script '" << m_name << "'";
				return Error;
			}
			if (!gc->initLuaScriptInstance( m_interp.get(), m_inputfilter, m_outputfilter))
			{
				LOG_ERROR << "error initializing lua script '" << m_name << "'";
				return Error;
			}
		}
		catch (const std::exception& e)
		{
			LOG_ERROR << "Failed to load script and initialize execution context: " << e.what();
			errorCode = "init script";
			return Error;
		}
		// call the function (for the first time)
		lua_getglobal( m_interp->thread(), m_name.c_str());
		std::vector<std::string>::const_iterator itr=m_argBuffer.begin(),end=m_argBuffer.end();
		for (;itr != end; ++itr)
		{
			lua_pushlstring( m_interp->thread(), itr->c_str(), itr->size());
		}
		nargs = (int)m_argBuffer.size();
	}
	do
	{
		// call the function (subsequently until termination)
		rt = lua_resume( m_interp->thread(), NULL, nargs);
		if (rt == LUA_YIELD)
		{
			if ((m_inputfilter.get() && m_inputfilter->state() != InputFilter::Open)
			||  (m_outputfilter.get() && m_outputfilter->state() != OutputFilter::Open))
			{
				return Yield;
			}
		}
		nargs = 0;
	}
	while (rt == LUA_YIELD);
	if (rt != 0)
	{
		const char* msg = lua_tostring( m_interp->thread(), -1);
		LOG_ERROR << "error calling function '" << m_name.c_str() << "':" << msg;
		errorCode = "lua call failed";
		return Error;
	}
	return Ok;
}


