cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(TESTS
  testSHA
  testMD5
  CRAMtest
  HMACtest
)

if(LONG_TESTS)
set(TESTS ${TESTS}
  PBKDF2test
  AES256test
)
endif()

include_directories(${Boost_INCLUDE_DIRS})

foreach(test ${TESTS})
  add_executable(${test} ${test}.cpp)
  target_link_libraries(${test} gtest ${CMAKE_THREAD_LIBS_INIT}
     ${Boost_LIBRARIES}
     wfsingleton wolframe provider
     wolframe_langbind
  )
  if(WIN32)
  add_custom_command(TARGET ${test} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${PROJECT_SOURCE_DIR}/src/libwfsingleton/wfsingleton.dll"
      $<TARGET_FILE_DIR:${test}>
  )
  endif()
  add_test(NAME ${test} COMMAND ${test})
endforeach(test)
