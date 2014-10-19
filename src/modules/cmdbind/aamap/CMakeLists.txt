cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_command_aamap_source_files
  aaMapProgram.cpp
  aaMapProgram.hpp
  mod_command_aamap.cpp
)

include_directories(
  "${Boost_INCLUDE_DIRS}"
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/3rdParty/textwolf/include"
)

add_library(mod_command_aamap MODULE ${mod_command_aamap_source_files})

set_target_properties(mod_command_aamap PROPERTIES PREFIX "")
