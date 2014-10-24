cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(createRandom_source_files
  createRandom.cpp
)

set(sqliteUnit_source_files
  sqliteUnit.cpp
)

if(WIN32)
  set(sqlite_includes "${PROJECT_SOURCE_DIR}/3rdParty/sqlite3")
else()
  set(sqlite_includes ${SQLITE3_INCLUDE_DIR})
endif()

include_directories(
  "${PROJECT_SOURCE_DIR}/src/modules/database/sqlite3"
  ${sqlite_includes}
)

add_executable(createRandom ${createRandom_source_files})

add_executable(sqliteUnit ${sqliteUnit_source_files} garbage.db test.db)

add_test(NAME sqliteUnit COMMAND sqliteUnit)

if(WIN32)
  set(sqlite_libraries sqlite3)
else()
  set(sqlite_libraries ${SQLITE3_LIBRARIES})
endif()

target_link_libraries(sqliteUnit gtest ${CMAKE_DL_LIBS}
  ${Boost_LIBRARIES}
  wfsingleton wolframe
  wolframe_database wolframe_serialize
  db_sqlite3_static
  ${sqlite_libraries}
)

if(WIN32)
add_custom_command(TARGET sqliteUnit POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/src/libwfsingleton/wfsingleton.dll"
    $<TARGET_FILE_DIR:sqliteUnit>
)
endif()

add_custom_command(OUTPUT garbage.db COMMAND createRandom 4096 garbage.db)

if(WIN32)
set(sqlite sqlite3shell)
else()
set(sqlite sqlite3)
endif()

add_custom_command(OUTPUT test.db COMMAND ${sqlite} test.db < ${PROJECT_SOURCE_DIR}/tests/database/sqlite3/testDB.sql)

