cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_protocol_wolframe_source_files
  authProtocolHandler.cpp
  mainProtocolHandler.hpp
  interfaceProtocolHandler.hpp
  ioblocks.cpp
  mod_protocol_wolframe.cpp
  lineProtocolHandler.hpp
  buffers.hpp
  passwordChangeProtocolHandler.cpp
  baseCryptoProtocolHandler.cpp
  iterators.hpp
  authProtocolHandler.hpp
  escDlfProtocolHandler.cpp
  passwordChangeProtocolHandler.hpp
  ioblocks.hpp
  mainProtocolHandler.cpp
  doctypeFilterProtocolHandler.cpp
  escDlfProtocolHandler.hpp
  doctypeFilterProtocolHandler.hpp
  lineProtocolHandler.cpp
  parser.hpp
  interfaceProtocolHandler.cpp
  baseCryptoProtocolHandler.hpp
)

include_directories(
  "${Boost_INCLUDE_DIRS}"
  "${PROJECT_SOURCE_DIR}/include"
)

add_library(mod_protocol_wolframe MODULE ${mod_protocol_wolframe_source_files})

set_target_properties(mod_protocol_wolframe PROPERTIES PREFIX "")
