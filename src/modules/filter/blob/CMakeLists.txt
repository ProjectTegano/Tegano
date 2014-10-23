cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_filter_blob_source_files
  mod_filter_blob.cpp
  blob_filter.cpp
  blob_filter.hpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
)

link_directories(${Boost_LIBRARY_DIRS})

add_library(mod_filter_blob MODULE ${mod_filter_blob_source_files})

set_target_properties(mod_filter_blob PROPERTIES PREFIX "")

target_link_libraries(mod_filter_blob
  wolframe
)
