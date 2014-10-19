cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(libcjson_source_files
  cJSON.h
  cJSON.c
)

add_definitions("-fPIC")

add_library(cjson STATIC ${libcjson_source_files})
  
