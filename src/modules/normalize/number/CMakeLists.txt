cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_normalize_number_source_files
  integerNormalizeFunction.hpp
  mod_normalize_number.cpp
  normalizeFunctionUtils.hpp
  floatNormalizeFunction.cpp
  floatNormalizeFunction.hpp
  normalizeFunctionUtils.cpp
  integerNormalizeFunction.cpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
)

link_directories(${Boost_LIBRARY_DIRS})

add_library(mod_normalize_number MODULE ${mod_normalize_number_source_files})

set_target_properties(mod_normalize_number PROPERTIES PREFIX "")

target_link_libraries(mod_normalize_number
  wolframe
)
