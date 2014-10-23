cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_datatype_datetime_source_files
  dateArithmetic.hpp
  mod_datatype_datetime.cpp
  datatypeDate.hpp
  dateArithmetic.cpp
  datatypeDate.cpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
)

add_library(mod_datatype_datetime MODULE ${mod_datatype_datetime_source_files})

set_target_properties(mod_datatype_datetime PROPERTIES PREFIX "")
