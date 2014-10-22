cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(libwolframe_langbind_source_files
  typedfilterScope.cpp
  output.cpp
  input.cpp
  filterdef.cpp
  joinfilter.cpp
  typingfilter.cpp
  execContextInputFilter.cpp
  null_filter.cpp
  ptreefilter.cpp
  ioFilterCommandHandler.cpp
  inputfilterScope.cpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include" 
  .
)

add_library(wolframe_langbind SHARED ${libwolframe_langbind_source_files})

target_link_libraries(wolframe_langbind wolframe)
