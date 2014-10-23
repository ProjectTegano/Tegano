cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(libwolframe_database_source_files
  baseStatement.cpp
  substitutingStatement.cpp
  vmTransactionOutput.cpp
  databaseLanguage.cpp
  transaction.cpp
  vm/programInstance.cpp
  vm/programImage.cpp
  vmTransactionInput.cpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
  .
)

link_directories(${Boost_LIBRARY_DIRS})

if(WIN32)
  add_library(wolframe_database STATIC ${libwolframe_database_source_files})
else()
  add_library(wolframe_database SHARED ${libwolframe_database_source_files})
endif()
