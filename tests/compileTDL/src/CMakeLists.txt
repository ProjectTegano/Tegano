cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

include_directories(
  "${PROJECT_SOURCE_DIR}/src/modules/cmdbind/tdl"
)

set(testCompileTDL_source_files
  compileTdlFiles.cpp
  testCompileTDL.cpp
)

add_executable(testCompileTDL ${testCompileTDL_source_files})

target_link_libraries(testCompileTDL gtest pthread
  ${Boost_LIBRARIES}
  wfsingleton wolframe
  wolframe_langbind wolframe_database
)

add_test(NAME testCompileTDL COMMAND testCompileTDL)
