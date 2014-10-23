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

add_library(wolframe_client SHARED ${libwolframe_client_source_files})
