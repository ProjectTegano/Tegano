cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_test_containers_source_files
  common.hpp
  mod_test_containers.hpp
  mod_test_containers.cpp
)

add_library(mod_test_containers MODULE ${mod_test_containers_source_files})

set_target_properties(mod_test_containers PROPERTIES PREFIX "")

target_link_libraries(mod_test_containers
  wfsingleton
)
