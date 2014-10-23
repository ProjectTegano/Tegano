cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

add_subdirectory(tests)

add_executable(moduleTest moduleTest.cpp)

target_link_libraries(moduleTest gtest ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS}
  ${Boost_LIBRARIES}
  wfsingleton wolframe provider config
  wolframe_langbind
)

add_test(NAME moduleTest COMMAND moduleTest)
