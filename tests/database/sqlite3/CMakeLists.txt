cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(createRandom_source_files
  createRandom.cpp
)

set(sqliteUnit_source_files
  sqliteUnit.cpp
)

include_directories(
  "${PROJECT_SOURCE_DIR}/src/modules/database/sqlite3"
)

add_executable(createRandom ${createRandom_source_files})

add_executable(sqliteUnit ${sqliteUnit_source_files} garbage.db test.db)

add_test(NAME sqliteUnit COMMAND sqliteUnit)

target_link_libraries(sqliteUnit gtest ${CMAKE_DL_LIBS}
  ${Boost_LIBRARIES}
  wfsingleton wolframe
  wolframe_database wolframe_serialize
  db_sqlite3_static
  sqlite3
)

add_custom_command(OUTPUT garbage.db COMMAND createRandom 4096 garbage.db)
add_custom_command(OUTPUT test.db COMMAND sqlite3 test.db < ${PROJECT_SOURCE_DIR}/tests/database/sqlite3/testDB.sql)
