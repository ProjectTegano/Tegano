cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(missing_symbol_source_files
  missing_symbol.cpp
)

add_library(missing_symbol MODULE ${missing_symbol_source_files})

set_target_properties(missing_symbol PROPERTIES PREFIX "")

if(WIN32)
target_link_libraries(missing_symbol
  bla
)
endif()
