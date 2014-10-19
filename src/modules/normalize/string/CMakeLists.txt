cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_normalize_string_source_files
  mod_normalize_string.cpp
  asciiNormalizeFunction.cpp
  trimNormalizeFunction.cpp
  trimNormalizeFunction.hpp
  asciiNormalizeFunction.hpp
)

include_directories(
  "${Boost_INCLUDE_DIRS}"
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/3rdParty/textwolf/include"
)

add_library(mod_normalize_string MODULE ${mod_normalize_string_source_files})

set_target_properties(mod_normalize_string PROPERTIES PREFIX "")
