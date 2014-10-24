cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_filter_testtoken_source_files
  testtoken_filter.cpp
  mod_filter_testtoken.cpp
  testtoken_filter.hpp
)

add_library(mod_filter_testtoken MODULE ${mod_filter_testtoken_source_files})

set_target_properties(mod_filter_testtoken PROPERTIES PREFIX "")

target_link_libraries(mod_filter_testtoken
  wolframe
)
