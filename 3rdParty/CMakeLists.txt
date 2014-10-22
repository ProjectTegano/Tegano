cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

add_subdirectory(gtest)

if(WITH_CJSON)
  add_subdirectory(libcjson)
endif()

if(WIN32)
if(WITH_SQLITE3)
  add_subdirectory(sqlite3)
endif()
endif()

if(WITH_LUA)
  add_subdirectory(lua)
endif()

if(WITH_LIBHPDF)
#  add_subdirectory(libhpdf)
endif()
