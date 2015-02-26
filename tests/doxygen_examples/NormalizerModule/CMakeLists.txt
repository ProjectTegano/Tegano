cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_normalizer_module_source_files
  src.cpp
)

include_directories(
  "${PROJECT_SOURCE_DIR}/include"
)

add_library(mod_normalizer_module MODULE ${mod_normalizer_module_source_files})

set_target_properties(mod_normalizer_module PROPERTIES PREFIX "")

target_link_libraries(mod_normalizer_module
  wolframe
)
