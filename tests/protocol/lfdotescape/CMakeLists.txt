cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

include_directories(
  "${PROJECT_SOURCE_DIR}/src/modules/protocol/wolframe"
)

set(protocolLFdotEscaping_source_files
   protocolLFdotEscaping.cpp
   protocolLFdotEscapingFiles.cpp
)

add_executable(protocolLFdotEscaping ${protocolLFdotEscaping_source_files})

target_link_libraries(protocolLFdotEscaping gtest pthread
  ${Boost_LIBRARIES}
  wfsingleton
)

add_test(NAME protocolLFdotEscaping COMMAND protocolLFdotEscaping)
