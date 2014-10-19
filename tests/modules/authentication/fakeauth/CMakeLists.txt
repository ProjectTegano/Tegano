cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_auth_test_source_files
  testAuthentication.cpp
  testAuthentication.hpp
  mod_auth_test.cpp
)

include_directories(
)

add_library(mod_auth_test MODULE ${mod_auth_test_source_files})

set_target_properties(mod_auth_test PROPERTIES PREFIX "")

target_link_libraries(mod_auth_test wolframe_serialize wolframe_langbind)
