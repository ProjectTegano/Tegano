cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(libsqlite3_source_files
  sqlite3.c
  sqlite3.h
  sqlite3ext.h
)

add_definitions("-fPIC")

add_library(sqlite3 STATIC ${libsqlite3_source_files})
  
