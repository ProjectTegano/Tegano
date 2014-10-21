cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_program_type_module_test_source_files
  src.cpp
)

include_directories(
  "${PROJECT_SOURCE_DIR}/include"
)

add_library(mod_program_type_module_test MODULE ${mod_program_type_module_test_source_files})

set_target_properties(mod_program_type_module_test PROPERTIES PREFIX "")
