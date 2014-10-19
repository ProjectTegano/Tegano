cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(resolvable_symbol_source_files
  resolvable_symbol.cpp
)

add_library(resolvable_symbol MODULE ${resolvable_symbol_source_files})

set_target_properties(resolvable_symbol PROPERTIES PREFIX "")

target_link_libraries(resolvable_symbol bla)

