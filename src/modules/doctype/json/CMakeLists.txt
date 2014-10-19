cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_doctype_json_source_files
  doctypeDetectorJson.hpp
  mod_doctype_json.cpp
  doctypeDetectorJson.cpp
)

include_directories(
  "${Boost_INCLUDE_DIRS}"
  "${PROJECT_SOURCE_DIR}/include"
)

add_library(mod_doctype_json MODULE ${mod_doctype_json_source_files})

set_target_properties(mod_doctype_json PROPERTIES PREFIX "")
