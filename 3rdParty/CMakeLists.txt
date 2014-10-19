cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

add_subdirectory(gtest)
#add_subdirectory(libhpdf)

if(WITH_CJSON)
  add_subdirectory(libcjson)
endif()

if(WITH_SQLITE3)
  add_subdirectory(sqlite3)
endif()

if(WITH_LUA)
add_subdirectory(lua)
endif()
