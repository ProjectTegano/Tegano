cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(libwfsingleton_source_files
  logger/logBackendConsole.cpp
  logger/logBackendConsole.hpp
  logger/logBackendFile.cpp
  logger/logBackendFile.hpp
  logger/logBackendImpl.cpp
  logger/logBackendImpl.hpp
  logger/logBackendSyslog.cpp
  logger/logBackendSyslog.hpp
  logger/logLevel.cpp
  logger/logSyslogFacility.cpp
  logger/logError.cpp
)

if(WIN32)
  set(libwfsingleton_source_files ${libwfsingleton_source_files}
    logger/logBackendEventlog.cpp
    logger/logBackendEventlog.hpp
    logger/logBackendWinDebug.cpp
    logger/logBackendWinDebug.hpp
    logger/syslog_WIN32.c
    logger/syslog_WIN32.h
    logger/wolframemsg.h
  )
endif()

if(WIN32)
  add_definitions("/DWFSHARED_DLL_IMPLEMENTATION")
endif()

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
)

link_directories(${Boost_LIBRARY_DIRS})

add_library(wfsingleton SHARED ${libwfsingleton_source_files})

if(WIN32)
  target_link_libraries(wfsingleton wolframe kernel32.lib ws2_32.lib)
endif()

add_custom_command(
  OUTPUT logger/wolframemsg.h
  COMMAND mc -h logger -r logger logger\\wolframemsg.mc
)
