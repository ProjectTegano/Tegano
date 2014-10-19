cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_doctype_textfl_source_files
  mod_doctype_textfl.cpp
  doctypeDetectorTest.hpp
  doctypeDetectorTest.cpp
)

add_library(mod_doctype_textfl MODULE ${mod_doctype_textfl_source_files})

set_target_properties(mod_doctype_textfl PROPERTIES PREFIX "")
