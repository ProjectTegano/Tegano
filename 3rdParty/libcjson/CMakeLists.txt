cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(libcjson_source_files
  cJSON.h
  cJSON.c
)

if(NOT WIN32)
  add_definitions("-fPIC")
endif()

add_library(cjson STATIC ${libcjson_source_files})
  
