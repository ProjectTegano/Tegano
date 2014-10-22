cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(db_postgresql_source_files
  PostgreSQLSubstitutingStatement.cpp
  PostgreSQLServerSettings.hpp
  PostgreSQL.cpp
  PostgreSQLServerSettings.cpp
  PostgreSQLDatatypes.cpp
  PostgreSQLDatatypes.hpp
  PostgreSQLSubstitutingStatement.hpp
  PostgreSQLStatement.hpp
  PostgreSQLConfig.hpp
  PostgreSQLTransactionExecStatemachine.hpp
  PostgreSQLConfig.cpp
  PostgreSQLTransactionExecStatemachine.cpp
  PostgreSQL.hpp
  PostgreSQLStatement.cpp
)

set(mod_db_postgresql_source_files
  mod_db_postgresql.cpp
  ${db_postgresql_source_files}
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  ${PostgreSQL_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/3rdParty/textwolf/include"
)

add_library(mod_db_postgresql MODULE ${mod_db_postgresql_source_files})

set_target_properties(mod_db_postgresql PROPERTIES PREFIX "")

target_link_libraries(mod_db_postgresql
  wolframe_database wolframe_serialize
  ${PostgreSQL_LIBRARIES}
)

add_definitions("-fPIC")

add_library(db_postgresql_static STATIC ${db_postgresql_source_files})

target_link_libraries(db_postgresql_static
  wolframe_database wolframe_serialize
  ${PostgreSQL_LIBRARIES}
)