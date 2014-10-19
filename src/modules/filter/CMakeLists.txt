cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

add_subdirectory(blob)

if(WITH_TEXTWOLF)
add_subdirectory(textwolf)
add_subdirectory(token)
add_subdirectory(char)
add_subdirectory(line)
endif()

if(WITH_LIBXML2)
add_subdirectory(libxml2)
endif()

if(WITH_CJSON)
add_subdirectory(cjson)
endif()
