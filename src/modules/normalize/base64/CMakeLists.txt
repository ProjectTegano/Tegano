cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_normalize_base64_source_files
  mod_normalize_base64.cpp
  base64NormalizeFunction.hpp
  base64NormalizeFunction.cpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
)

link_directories(${Boost_LIBRARY_DIRS})

add_library(mod_normalize_base64 MODULE ${mod_normalize_base64_source_files})

set_target_properties(mod_normalize_base64 PROPERTIES PREFIX "")

target_link_libraries(mod_normalize_base64
  wolframe
)
