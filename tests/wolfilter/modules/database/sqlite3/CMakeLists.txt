cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_db_sqlite3test_source_files
  SQLiteTest.hpp
  mod_db_sqlite3test.cpp
  SQLiteTest.cpp
)

include_directories(
  ${SQLITE3_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/src/modules/database/sqlite3"
)

add_library(mod_db_sqlite3test MODULE ${mod_db_sqlite3test_source_files})

set_target_properties(mod_db_sqlite3test PROPERTIES PREFIX "")

target_link_libraries(mod_db_sqlite3test db_sqlite3_static ${SQLITE3_LIBRARIES})

target_link_libraries(mod_db_sqlite3test wolframe_database)
