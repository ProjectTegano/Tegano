cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

add_subdirectory(directmap)
add_subdirectory(tdl)
add_subdirectory(aamap)

if(WITH_LUA)
  add_subdirectory(lua)
endif()

if(WIN32)
if(WITH_DOTNET)
  add_subdirectory(dotnet)
endif()
endif()
