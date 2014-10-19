cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(wolfilter_source_files
  contentOnlyProtocolHandler.hpp
  wolfilterIostreamFilter.hpp
  wolfilterCommandLine.hpp
  contentOnlyProtocolHandler.cpp
  wolfilterIostreamFilter.cpp
  wolfilterCommandLine.cpp
  wolfilter.cpp
)

include_directories(
  "${Boost_INCLUDE_DIRS}"
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/src"
)

add_executable(wolfilter ${wolfilter_source_files})

target_link_libraries(wolfilter ${Boost_LIBRARIES} ${CMAKE_DL_LIBS}
  config wfsingleton wolframe provider
  wolframe_serialize wolframe_langbind
)
