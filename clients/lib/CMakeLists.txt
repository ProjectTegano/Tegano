cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(libwolframe_client_source_files
  connection.cpp
  session.cpp
  session.hpp
  exportable.hpp
  protocol.cpp
  protocol.hpp
  connection.hpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})

if(WIN32)
  add_library(wolframe_client STATIC ${libwolframe_client_source_files})
else()
  add_library(wolframe_client SHARED ${libwolframe_client_source_files})
endif()
