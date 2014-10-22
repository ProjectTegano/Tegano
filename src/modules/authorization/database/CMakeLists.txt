cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_authz_database_source_files
  DBauthz.hpp
  mod_authz_database.cpp
  DBauthzConfig.cpp
  DBauthz.cpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
)

add_library(mod_authz_database MODULE ${mod_authz_database_source_files})

set_target_properties(mod_authz_database PROPERTIES PREFIX "")
