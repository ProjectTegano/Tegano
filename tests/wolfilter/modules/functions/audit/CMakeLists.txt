cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_audit_source_files
  mod_audit.cpp
  audit.cpp
  audit.hpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
)

add_library(mod_audit MODULE ${mod_audit_source_files})

set_target_properties(mod_audit PROPERTIES PREFIX "")
