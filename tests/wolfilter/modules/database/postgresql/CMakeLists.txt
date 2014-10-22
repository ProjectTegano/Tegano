cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_db_postgresqltest_source_files
  PostgreSQLTest.hpp
  PostgreSQLTest.cpp
  mod_db_postgresqltest.cpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  ${PostgreSQL_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/src/modules/database/postgresql"
)

add_library(mod_db_postgresqltest MODULE ${mod_db_postgresqltest_source_files})

set_target_properties(mod_db_postgresqltest PROPERTIES PREFIX "")

target_link_libraries(mod_db_postgresqltest db_postgresql_static)

target_link_libraries(mod_db_postgresqltest wolframe_database)
