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

if(WIN32)
  set(sqlite_includes "${PROJECT_SOURCE_DIR}/3rdParty/sqlite3")
else()
  set(sqlite_includes ${SQLITE3_INCLUDE_DIR})
endif()

include_directories(
  ${sqlite_includes}
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/3rdParty/textwolf/include"
)

link_directories(${Boost_LIBRARY_DIRS})

add_library(mod_db_sqlite3 MODULE ${mod_db_sqlite3_source_files})

set_target_properties(mod_db_sqlite3 PROPERTIES PREFIX "")

if(WIN32)
  set(sqlite_libraries sqlite3)
else()
  set(sqlite_libraries ${SQLITE3_LIBRARIES})
endif()

target_link_libraries(mod_db_sqlite3
  wolframe_database wolframe_serialize
  ${sqlite_libraries}
)

if(NOT WIN32)
  add_definitions("-fPIC")
endif()

add_library(db_sqlite3_static STATIC ${db_sqlite3_source_files})

target_link_libraries(db_sqlite3_static ${CMAKE_DL_LIBS}
  ${sqlite_libraries}
  wolframe_database wolframe_serialize
)
