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

add_test(NAME testSimpleFormCompiler COMMAND testSimpleFormCompiler)
