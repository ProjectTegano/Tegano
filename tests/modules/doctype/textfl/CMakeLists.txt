cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_doctype_textfl_source_files
  mod_doctype_textfl.cpp
  doctypeDetectorTest.hpp
  doctypeDetectorTest.cpp
)

link_directories(${Boost_LIBRARY_DIRS})

add_library(mod_doctype_textfl MODULE ${mod_doctype_textfl_source_files})

set_target_properties(mod_doctype_textfl PROPERTIES PREFIX "")

target_link_libraries(mod_doctype_textfl
  wolframe
)
