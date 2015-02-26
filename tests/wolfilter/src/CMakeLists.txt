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

if(WIN32)
add_custom_command(TARGET testWolfilter POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/src/libwfsingleton/wfsingleton.dll"
    $<TARGET_FILE_DIR:testWolfilter>
)
endif()

add_test(NAME testWolfilter COMMAND testWolfilter)
