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

include_directories("${PROJECT_SOURCE_DIR}/include" .)

add_library(wolframe_database SHARED ${libwolframe_database_source_files})
