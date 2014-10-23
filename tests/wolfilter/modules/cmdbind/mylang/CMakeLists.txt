cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_command_mylang_source_files
  mylangStructureBuilder.hpp
  mod_command_mylang.cpp
  mylangFunctionProgramType.hpp
  mylangStructureBuilder.cpp
  mylangFunctionProgramType.cpp
  mylangInterpreterInstance.cpp
  mylangStructure.hpp
  mylangStructure.cpp
  mylangInterpreterInstance.hpp
  mylangInterpreter.cpp
  mylangInterpreter.hpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
)

add_library(mod_command_mylang MODULE ${mod_command_mylang_source_files})

set_target_properties(mod_command_mylang PROPERTIES PREFIX "")
