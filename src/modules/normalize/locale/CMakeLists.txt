cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_normalize_locale_source_files
  localeNormalize.hpp
  mod_normalize_locale.cpp
  localeNormalize.cpp
)

include_directories(
  "${PROJECT_SOURCE_DIR}/include"
)

add_library(mod_normalize_locale MODULE ${mod_normalize_locale_source_files})

set_target_properties(mod_normalize_locale PROPERTIES PREFIX "")
