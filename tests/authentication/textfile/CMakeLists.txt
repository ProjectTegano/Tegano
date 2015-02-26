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
  target_link_libraries(${test} gtest ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS}
    ${Boost_LIBRARIES}
    config wfsingleton wolframe provider
    wolframe_langbind
    auth_textfile_static
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
