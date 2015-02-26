cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

include_directories(
  "${PROJECT_SOURCE_DIR}/src/modules/protocol/wolframe"
)
set(testProtocol_source_files
   testProtocol.cpp
)

add_executable(testProtocol ${testProtocol_source_files})

target_link_libraries(testProtocol gtest ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS}
  ${Boost_LIBRARIES}
  config wfsingleton wolframe provider
  wolframe_langbind wolframe_serialize
)

if(WIN32)
add_custom_command(TARGET testProtocol POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/src/libwfsingleton/wfsingleton.dll"
    $<TARGET_FILE_DIR:testProtocol>
)
endif()

add_test(NAME testProtocol COMMAND testProtocol)
