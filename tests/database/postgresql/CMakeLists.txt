cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(dbPing_source_files
  dbPing.cpp
)

set(postgresUnit_source_files
  postgresUnit.cpp
)

include_directories(
  ${PostgreSQL_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/src/modules/database/postgresql"
)

add_executable(dbPing ${dbPing_source_files})

target_link_libraries(dbPing
  ${Boost_LIBRARIES}
  wfsingleton
  ${PostgreSQL_LIBRARIES}
)

add_executable(postgresUnit ${postgresUnit_source_files})

#TODO: ./dbPing localhost wolfusr wolfpwd wolframe as condition to execute the test
add_test(NAME postgresUnit COMMAND postgresUnit)

target_link_libraries(postgresUnit gtest ${CMAKE_DL_LIBS}
  ${Boost_LIBRARIES}
  wfsingleton wolframe
  wolframe_database wolframe_serialize
  db_postgresql_static
  ${PostgreSQL_LIBRARIES}
)
