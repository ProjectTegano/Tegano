cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_filter_line_source_files
  line_filter.cpp
  mod_filter_line.cpp
  line_filter.hpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/3rdParty/textwolf/include"
)

add_library(mod_filter_line MODULE ${mod_filter_line_source_files})

set_target_properties(mod_filter_line PROPERTIES PREFIX "")
