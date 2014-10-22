cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(testWolfilter_source_files
  testWolfilter.cpp
  testWolfilterFiles.cpp
)

include_directories(
   ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/src/wolfilter"
)

add_executable(testWolfilter ${testWolfilter_source_files})

target_link_libraries(testWolfilter
   gtest wtest
   ${Boost_LIBRARIES} ${CMAKE_DL_LIBS}
   config wfsingleton wolframe provider
   wolframe_langbind wolframe_serialize wolframe_database
)

add_test(NAME testWolfilter COMMAND testWolfilter)
