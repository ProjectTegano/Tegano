cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

add_subdirectory(testtrace)

if(WITH_PGSQL)
  add_subdirectory(postgresql)
endif()

if(WITH_SQLITE3)
  add_subdirectory(sqlite3)
endif()
