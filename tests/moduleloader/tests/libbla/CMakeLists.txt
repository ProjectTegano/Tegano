cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(libbla_source_files
  bla.cpp
)

add_library(bla SHARED ${libbla_source_files})

