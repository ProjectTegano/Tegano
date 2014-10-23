cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_filter_char_source_files
  char_filter.cpp
  char_filter.hpp
  mod_filter_char.cpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/3rdParty/textwolf/include"
)

link_directories(${Boost_LIBRARY_DIRS})

add_library(mod_filter_char MODULE ${mod_filter_char_source_files})

set_target_properties(mod_filter_char PROPERTIES PREFIX "")

target_link_libraries(mod_filter_char
  wolframe
)
