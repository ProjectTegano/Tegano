cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

add_subdirectory(number)
add_subdirectory(base64)

if(WITH_TEXTWOLF)
  add_subdirectory(string)
endif()

if(WITH_ICU)
  add_subdirectory(locale)
endif()
