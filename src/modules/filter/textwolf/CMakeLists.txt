cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_filter_textwolf_source_files
  mod_filter_textwolf.cpp
  textwolf_filter.hpp
  textwolf_filter.cpp
)

include_directories(
  "${Boost_INCLUDE_DIRS}"
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/3rdParty/textwolf/include"
)

add_library(mod_filter_textwolf MODULE ${mod_filter_textwolf_source_files})

set_target_properties(mod_filter_textwolf PROPERTIES PREFIX "")
