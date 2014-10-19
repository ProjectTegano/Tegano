cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

include_directories(
  "${PROJECT_SOURCE_DIR}/src/modules/protocol/wolframe"
)
set(testProtocol_source_files
   testProtocol.cpp
)

add_executable(testProtocol ${testProtocol_source_files})

target_link_libraries(testProtocol gtest pthread ${CMAKE_DL_LIBS}
  ${Boost_LIBRARIES}
  config wfsingleton wolframe provider
  wolframe_langbind wolframe_serialize
)

add_test(NAME testProtocol COMMAND testProtocol)
