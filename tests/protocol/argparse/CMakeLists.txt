cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

include_directories(
  "${PROJECT_SOURCE_DIR}/src/modules/protocol/wolframe"
)

set(protocolArgumentParsing_source_files
   protocolArgumentParsing.cpp
   protocolArgumentParsingFiles.cpp
)

add_executable(protocolArgumentParsing ${protocolArgumentParsing_source_files})

target_link_libraries(protocolArgumentParsing gtest ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_LIBRARIES}
  wfsingleton
)

if(WIN32)
add_custom_command(TARGET protocolArgumentParsing POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/src/libwfsingleton/wfsingleton.dll"
    $<TARGET_FILE_DIR:protocolArgumentParsing>
)
endif()

add_test(NAME protocolArgumentParsing COMMAND protocolArgumentParsing)
