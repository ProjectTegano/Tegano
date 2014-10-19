cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(not_a_mod_source_files
  not_a_mod.cpp
)

add_library(not_a_mod MODULE ${not_a_mod_source_files})

set_target_properties(not_a_mod PROPERTIES PREFIX "")

