cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_cmdbind_echo_source_files
  echoCommandHandler.cpp
  echoCommandHandler.hpp
  echoCommandHandlerConfig.cpp
  echoCommandHandlerConfig.hpp
  echoCommandHandlerUnit.cpp
  echoCommandHandlerUnit.hpp
  mod_cmdbind_echo.cpp
)

add_library(mod_cmdbind_echo MODULE ${mod_cmdbind_echo_source_files})

set_target_properties(mod_cmdbind_echo PROPERTIES PREFIX "")

target_link_libraries(mod_cmdbind_echo wolframe_serialize wolframe_langbind)
