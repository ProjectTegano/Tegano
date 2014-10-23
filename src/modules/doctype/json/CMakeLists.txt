cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_doctype_json_source_files
  doctypeDetectorJson.hpp
  mod_doctype_json.cpp
  doctypeDetectorJson.cpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
)

link_directories(${Boost_LIBRARY_DIRS})

add_library(mod_doctype_json MODULE ${mod_doctype_json_source_files})

set_target_properties(mod_doctype_json PROPERTIES PREFIX "")

target_link_libraries(mod_doctype_json
  wfsingleton
)
