cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_command_lua_source_files
  luaCommandHandlerUnit.hpp
  luaCppCall.hpp
  luaGetFunctionClosure.cpp
  luaDump.cpp
  luaObjects.cpp
  luaObjectTemplate.hpp
  luaDump.hpp
  luaObjects.hpp
  luaDebug.hpp
  luaScriptContext.cpp
  luaGetFunctionClosure.hpp
  luafilter.cpp
  luaFunctionProgramType.hpp
  luaScriptContext.hpp
  luaException.hpp
  luaCommandHandler.hpp
  luaCommandHandler.cpp
  luaFunctionProgramType.cpp
  luaCommandHandlerUnit.cpp
  luaCommandHandlerConfig.cpp
  luafilter.hpp
  luaDebug.cpp
  mod_command_lua.cpp
  luaCommandHandlerConfig.hpp
)

include_directories(
  "${Boost_INCLUDE_DIRS}"
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/3rdParty/lua/src"
)

add_definitions("-DWITH_LUA")

add_library(mod_command_lua MODULE ${mod_command_lua_source_files})

set_target_properties(mod_command_lua PROPERTIES PREFIX "")

target_link_libraries(mod_command_lua lua wolframe_serialize)
