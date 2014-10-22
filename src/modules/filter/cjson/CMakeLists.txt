cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_filter_cjson_source_files
  inputfilterImpl.hpp
  mod_filter_cjson.cpp
  outputfilterImpl.hpp
  inputfilterImpl.cpp
  cjson_filter.cpp
  outputfilterImpl.cpp
  cjson_filter.hpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/3rdParty/libcjson"
)

add_library(mod_filter_cjson MODULE ${mod_filter_cjson_source_files})

set_target_properties(mod_filter_cjson PROPERTIES PREFIX "")

target_link_libraries(mod_filter_cjson cjson)
