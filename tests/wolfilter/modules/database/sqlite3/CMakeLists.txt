cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_db_sqlite3test_source_files
  SQLiteTest.hpp
  mod_db_sqlite3test.cpp
  SQLiteTest.cpp
)

if(WIN32)
  set(sqlite_includes "${PROJECT_SOURCE_DIR}/3rdParty/sqlite3")
else()
  set(sqlite_includes ${SQLITE3_INCLUDE_DIR})
endif()

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/src/modules/database/sqlite3"
  ${sqlite_includes}
)

add_library(mod_db_sqlite3test MODULE ${mod_db_sqlite3test_source_files})

set_target_properties(mod_db_sqlite3test PROPERTIES PREFIX "")

if(WIN32)
  set(sqlite_libraries sqlite3)
else()
  set(sqlite_libraries ${SQLITE3_LIBRARIES})
endif()

target_link_libraries(mod_db_sqlite3test db_sqlite3_static ${SQLITE3_LIBRARIES})

target_link_libraries(mod_db_sqlite3test
  wolframe_database
  ${sqlite_libraries}
)
