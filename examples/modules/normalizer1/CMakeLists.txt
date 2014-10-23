cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_example_normalize1_source_files
  mod_example_normalize1.cpp
)

include_directories(
  "${PROJECT_SOURCE_DIR}/include"
)

add_library(mod_example_normalize1 MODULE ${mod_example_normalize1_source_files})

set_target_properties(mod_example_normalize1 PROPERTIES PREFIX "")
