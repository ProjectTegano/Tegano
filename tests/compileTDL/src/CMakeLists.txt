cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

include_directories(
  "${PROJECT_SOURCE_DIR}/src/modules/cmdbind/tdl"
)

set(testCompileTDL_source_files
  compileTdlFiles.cpp
  testCompileTDL.cpp
)

add_executable(testCompileTDL ${testCompileTDL_source_files})

target_link_libraries(testCompileTDL gtest ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_LIBRARIES}
  wfsingleton wolframe
  wolframe_langbind wolframe_database
)
if(WIN32)
add_custom_command(TARGET testCompileTDL POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/src/libwfsingleton/wfsingleton.dll"
    $<TARGET_FILE_DIR:testCompileTDL>
)
endif()

add_test(NAME testCompileTDL COMMAND testCompileTDL)
