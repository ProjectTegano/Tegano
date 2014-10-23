cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_ddlcompiler_simpleform_source_files
  mod_ddlcompiler_simpleform.cpp
  simpleFormCompiler.hpp
  simpleFormLexer.hpp
  simpleFormLexer.cpp
  simpleFormCompiler.cpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
)

add_library(mod_ddlcompiler_simpleform MODULE ${mod_ddlcompiler_simpleform_source_files})

set_target_properties(mod_ddlcompiler_simpleform PROPERTIES PREFIX "")
