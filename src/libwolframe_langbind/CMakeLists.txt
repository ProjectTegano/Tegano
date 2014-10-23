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

link_directories(${Boost_LIBRARY_DIRS})

if(WIN32)
  add_library(wolframe_langbind STATIC ${libwolframe_langbind_source_files})
else()
  add_library(wolframe_langbind SHARED ${libwolframe_langbind_source_files})
endif()

target_link_libraries(wolframe_langbind wolframe wfsingleton)
