cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_test_source_files
  common.hpp
  mod_test.hpp
  mod_test.cpp
)

add_library(mod_test MODULE ${mod_test_source_files})

set_target_properties(mod_test PROPERTIES PREFIX "")

target_link_libraries(mod_test
  wfsingleton
)
