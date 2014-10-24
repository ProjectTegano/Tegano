cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

add_subdirectory(tests)

add_executable(moduleTest moduleTest.cpp)

target_link_libraries(moduleTest gtest ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS}
  ${Boost_LIBRARIES}
  wfsingleton wolframe provider config
  wolframe_langbind
)

add_custom_command(TARGET moduleTest POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/src/libwfsingleton/wfsingleton.dll"
    $<TARGET_FILE_DIR:moduleTest>
)

add_test(NAME moduleTest COMMAND moduleTest)
