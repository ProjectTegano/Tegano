cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_db_testtrace_source_files
  mod_db_testtrace.cpp
  testtraceDatabase.cpp
  testtraceTransaction.hpp
  testtraceTransaction.cpp
  testtraceDatabase.hpp
)

add_library(mod_db_testtrace MODULE ${mod_db_testtrace_source_files})

set_target_properties(mod_db_testtrace PROPERTIES PREFIX "")
