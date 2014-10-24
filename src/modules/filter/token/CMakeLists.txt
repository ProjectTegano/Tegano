cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_filter_token_source_files
  mod_filter_token.cpp
  token_filter.hpp
  token_filter.cpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/3rdParty/textwolf/include"
)

link_directories(${Boost_LIBRARY_DIRS})

add_library(mod_filter_token MODULE ${mod_filter_token_source_files})

set_target_properties(mod_filter_token PROPERTIES PREFIX "")

target_link_libraries(mod_filter_token
  wolframe
)
