cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_audit_textfile_source_files
  TextFileAuditConfig.cpp
  TextFileAudit.cpp
  TextFileAudit.hpp
  mod_audit_textfile.cpp
)

include_directories(
  "${Boost_INCLUDE_DIRS}"
  "${PROJECT_SOURCE_DIR}/include"
)

add_library(mod_audit_textfile MODULE ${mod_audit_textfile_source_files})

set_target_properties(mod_audit_textfile PROPERTIES PREFIX "")
