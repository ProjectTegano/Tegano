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
)

if(win32)
  set(libwfsingleton_source_files ${libwfsingleton_source_files}
    logger/logBackendEventlog.cpp
    logger/logBackendEventlog.hpp
    logger/logBackendWinDebug.cpp
    logger/logBackendWinDebug.hpp
    logger/syslog_win32.c
    logger/syslog_win32.h
  )
endif(win32)

if(win32)
  add_definitions("-DWFSHARED_DLL_IMPLEMENTATION")
endif(win32)

include_directories("${PROJECT_SOURCE_DIR}/include")

add_library(wfsingleton SHARED ${libwfsingleton_source_files})
