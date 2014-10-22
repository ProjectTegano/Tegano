cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_datatype_bcdnumber_source_files
  bcdArithmetic.hpp
  fxpArithmetic.cpp
  bcdArithmetic.cpp
  datatypeBigfxp.cpp
  mod_datatype_bcdnumber.cpp
  fxpArithmetic.hpp
  datatypeBigfxp.hpp
  datatypeBigint.cpp
  datatypeBigint.hpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
)

add_library(mod_datatype_bcdnumber MODULE ${mod_datatype_bcdnumber_source_files})

set_target_properties(mod_datatype_bcdnumber PROPERTIES PREFIX "")
