cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(TESTS
  textFileAuthenticator
  passwdHashTest
  textFileThrow
  textFileTest
  textFilePwdChange
  passwdFileTest
)

# TODO: disabled, dest fails: textFileTestFiles

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/src/modules/authentication/textfile"
)

foreach(test ${TESTS})
  add_executable(${test} ${test}.cpp)
  target_link_libraries(${test} gtest pthread ${CMAKE_DL_LIBS}
    ${Boost_LIBRARIES}
    config wfsingleton wolframe provider
    wolframe_langbind
    auth_textfile_static
  )
  add_test(NAME ${test} COMMAND ${test})
endforeach(test)
