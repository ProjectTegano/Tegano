cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(testSimpleFormCompiler_source_files
  normalizeFunctions.cpp
  testSimpleFormCompiler.cpp
  simpleFormCompiler.cpp
  normalizeFunctions.cpp
  testSimpleFormCompiler.cpp
  simpleFormCompiler.cpp
)

add_executable(testSimpleFormCompiler ${testSimpleFormCompiler_source_files})

target_link_libraries(testSimpleFormCompiler gtest
  ${Boost_LIBRARIES}
  wolframe
)

if(WIN32)
add_custom_command(TARGET testSimpleFormCompiler POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/src/libwfsingleton/wfsingleton.dll"
    $<TARGET_FILE_DIR:testSimpleFormCompiler>
)
endif()

add_test(NAME testSimpleFormCompiler COMMAND testSimpleFormCompiler)
