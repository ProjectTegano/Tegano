cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_doctype_xml_source_files
  mod_doctype_xml.cpp
  doctypeDetectorXml.cpp
  doctypeDetectorXml.hpp
)

include_directories(
  "${Boost_INCLUDE_DIRS}"
  "${PROJECT_SOURCE_DIR}/include"
)

add_library(mod_doctype_xml MODULE ${mod_doctype_xml_source_files})

set_target_properties(mod_doctype_xml PROPERTIES PREFIX "")
