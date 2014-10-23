cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_audit_database_source_files
  DBaudit.hpp
  DBaudit.cpp
  DBauditConfig.cpp
  mod_audit_database.cpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
)

link_directories(${Boost_LIBRARY_DIRS})

add_library(mod_audit_database MODULE ${mod_audit_database_source_files})

set_target_properties(mod_audit_database PROPERTIES PREFIX "")

target_link_libraries(mod_audit_database
  wfsingleton provider
)
