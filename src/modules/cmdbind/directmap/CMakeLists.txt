cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_command_directmap_source_files
  directmapCommandHandlerUnit.hpp
  directmapProgram.hpp
  directmapCommandHandler.cpp
  directmapCommandHandler.hpp
  directmapCommandHandlerUnit.cpp
  directmapCommandDescription.cpp
  mod_command_directmap.cpp
  directmapCommandHandlerConfig.cpp
  directmapCommandHandlerConfig.hpp
  directmapProgram.cpp
  directmapCommandDescription.hpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
)

add_library(mod_command_directmap MODULE ${mod_command_directmap_source_files})

set_target_properties(mod_command_directmap PROPERTIES PREFIX "")

target_link_libraries(mod_command_directmap 
  wolframe_serialize
)
