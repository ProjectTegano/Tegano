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
  target_link_libraries(${test} gtest pthread
     ${Boost_LIBRARIES}
     wfsingleton wolframe provider
     wolframe_langbind
  )
  add_test(NAME ${test} COMMAND ${test})
endforeach(test)
