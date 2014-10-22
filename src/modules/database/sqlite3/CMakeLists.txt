cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(db_sqlite3_source_files
  SQLiteTransactionExecStatemachine.cpp
  SQLiteConfig.hpp
  SQLiteConfig.cpp
  SQLite.cpp
  SQLiteTransactionExecStatemachine.hpp
  SQLiteStatement.cpp
  SQLite.hpp
  SQLiteStatement.hpp
)

set(mod_db_sqlite3_source_files
  mod_db_sqlite3.cpp
  ${db_sqlite3_source_files}
)

include_directories(
  ${SQLITE3_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/3rdParty/textwolf/include"
)

add_library(mod_db_sqlite3 MODULE ${mod_db_sqlite3_source_files})

set_target_properties(mod_db_sqlite3 PROPERTIES PREFIX "")

target_link_libraries(mod_db_sqlite3
  wolframe_database wolframe_serialize
  ${SQLITE3_LIBRARIES}
)

add_definitions("-fPIC")

add_library(db_sqlite3_static STATIC ${db_sqlite3_source_files})

target_link_libraries(db_sqlite3_static ${CMAKE_DL_LIBS}
  sqlite3
  wolframe_database wolframe_serialize
)
