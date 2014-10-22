cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_auth_database_source_files
  DBauth.cpp
  mod_auth_database.cpp
  DBauth.hpp
  DBauthConfig.cpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
)

add_library(mod_auth_database MODULE ${mod_auth_database_source_files})

set_target_properties(mod_auth_database PROPERTIES PREFIX "")
