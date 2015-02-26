cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

include_directories(
  "${PROJECT_SOURCE_DIR}/src/modules/protocol/wolframe"
)

set(protocolLFdotEscaping_source_files
   protocolLFdotEscaping.cpp
   protocolLFdotEscapingFiles.cpp
)

add_executable(protocolLFdotEscaping ${protocolLFdotEscaping_source_files})

target_link_libraries(protocolLFdotEscaping gtest ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_LIBRARIES}
  wfsingleton
)

if(WIN32)
add_custom_command(TARGET protocolLFdotEscaping POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/src/libwfsingleton/wfsingleton.dll"
    $<TARGET_FILE_DIR:protocolLFdotEscaping>
)
endif()

add_test(NAME protocolLFdotEscaping COMMAND protocolLFdotEscaping)
