cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_example_normalize2_source_files
  mod_example_normalize2.cpp
)

include_directories(
  "${PROJECT_SOURCE_DIR}/include"
)

add_library(mod_example_normalize2 MODULE ${mod_example_normalize2_source_files})

set_target_properties(mod_example_normalize2 PROPERTIES PREFIX "")
