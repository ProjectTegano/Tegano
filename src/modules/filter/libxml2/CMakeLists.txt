cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_filter_libxml2_source_files
  inputfilterImpl.hpp
  documentReader.hpp
  xsltMapper.cpp
  outputfilterImpl.hpp
  xsltMapper.hpp
  xsltProgramType.cpp
  inputfilterImpl.cpp
  documentReader.cpp
  libxml2_filter.hpp
  xsltProgramType.hpp
  documentWriter.hpp
  outputfilterImpl.cpp
  libxml2_filter.cpp
  mod_filter_libxml2.cpp
  documentWriter.cpp
)

include_directories(
  "${LIBXML2_INCLUDE_DIR}"
  "${LIBXSLT_INCLUDE_DIR}"
  "${Boost_INCLUDE_DIRS}"
  "${PROJECT_SOURCE_DIR}/include"
)

add_library(mod_filter_libxml2 MODULE ${mod_filter_libxml2_source_files})

set_target_properties(mod_filter_libxml2 PROPERTIES PREFIX "")

target_link_libraries(mod_filter_libxml2 ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES})
