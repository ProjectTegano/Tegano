cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(libwtest_source_files
  testModules.cpp
  testDescription.cpp 
  include/wtest/testHandlerTemplates.hpp
  include/wtest/pseudoRandomGenForTests.hpp
  include/wtest/testDescription.hpp
  include/wtest/testModules.hpp
  include/wtest/testReport.hpp
)

if(WITH_SSL)
  add_definitions("-DWITH_SSL")
endif()

if(WITH_TEXTWOLF)
  add_definitions("-DWITH_TEXTWOLF")
endif()

if(WITH_LUA)
  add_definitions("-DWITH_LUA")
endif()

if(WIN32)
if(WITH_DOTNET)
#  add_definitions("-DWITH_DOTNET")
endif()
endif()

if(WITH_PGSQL)
#  add_definitions("-DWITH_PGSQL")
endif()

if(WITH_SQLITE3)
  add_definitions("-DWITH_SQLITE3")
endif()

if(WITH_LIBHPDF)
#  add_definitions("-DWITH_LIBHPDF")
endif()

if(WITH_LIBXML2)
  add_definitions("-DWITH_LIBXML2")
endif()

if(WITH_LIBXSLT)
  add_definitions("-DWITH_LIBXSLT")
endif()

if(WITH_ICU)
#  add_definitions("-DWITH_ICU")
endif()

if(WITH_CJSON)
  add_definitions("-DWITH_CJSON")
endif()

include_directories(
  "${Boost_INCLUDE_DIRS}"
  "${PROJECT_SOURCE_DIR}/wtest/include"
)

add_library(wtest STATIC ${libwtest_source_files})

add_executable(cleaninput cleanInput.cpp)

add_executable(wtestcheck wtestcheck.cpp)

target_link_libraries(wtestcheck wtest ${Boost_LIBRARIES})
