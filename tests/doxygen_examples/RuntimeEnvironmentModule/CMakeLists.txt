cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_runtime_environment_module_test_source_files
  src.cpp
)

include_directories(
  "${PROJECT_SOURCE_DIR}/include"
)

add_library(mod_runtime_environment_module_test MODULE ${mod_runtime_environment_module_test_source_files})

set_target_properties(mod_runtime_environment_module_test PROPERTIES PREFIX "")

target_link_libraries(mod_runtime_environment_module_test
  wfsingleton wolframe
  wolframe_serialize
)
