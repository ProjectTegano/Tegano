cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(Tegano)
set(TEGANO_MAJOR_VERSION 0)
set(TEGANO_MINOR_VERSION 0)
set(TEGANO_PATCH_VERSION 3)
set(TEGANO_VERSION ${TEGANO_MAJOR_VERSION}.${TEGANO_MINOR_VERSION}.${TEGANO_PATCH_VERSION})

include(cmake/LinuxDistribution.cmake)

# TODO: needed for packaging only

# ifdef DEFAULT_MODULE_LOAD_DIR
# INCLUDE_CXXFLAGS += -DDEFAULT_MODULE_LOAD_DIR=$(DEFAULT_MODULE_LOAD_DIR)
# endif
# set default main configuration file location
# ifdef DEFAULT_MAIN_CONFIGURATION_FILE
# INCLUDE_CXXFLAGS += -DDEFAULT_MAIN_CONFIGURATION_FILE=$(DEFAULT_MAIN_CONFIGURATION_FILE)
# endif

set_property(GLOBAL PROPERTY rule_launch_compile ccache)
set_property(GLOBAL PROPERTY rule_launch_link ccache)

if(CMAKE_COMPILER_IS_GNUCXX)
set(CMAKE_CXX_FLAGS "-std=c++98 -Wall")
set(CMAKE_C_FLAGS "-std=c99 -Wall -pedantic")
endif()

if(WIN32)
set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} /D_WIN32_WINNT=0x0504")
set(CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} /D_WIN32_WINNT=0x0504")
endif()

# TODO: configurable
set(WITH_TEXTWOLF 1)
set(WITH_SSL 1)
set(WITH_LUA 1)
if(WIN32)
set(WITH_DOTNET 1)
endif()
set(WITH_PGSQL 1)
set(WITH_SQLITE3 1)
set(WITH_LIBXML2 1)
set(WITH_LIBXSLT 1)
set(WITH_CJSON 1)
set(WITH_ICU 1)
set(WITH_LIBHPDF 1)
set(LONG_TESTS 0)

if(WITH_ICU)
  set(optional_boost_packages locale)
endif()

find_package(Threads)
find_package(Boost 1.48.0 COMPONENTS thread filesystem system program_options date_time ${optional_boost_packages})

if(WITH_SSL)
find_package(OpenSSL)
endif()

if(WITH_LIBXML2)
find_package(LibXml2)
endif()

if(WITH_LIBXSLT)
find_package(LibXslt)
endif()

set(CMAKE_MODULE_PATH cmake)

if(WITH_PGSQL)
# broken on Ubuntu 12.04!
if(${INSTALLER_PLATFORM} STREQUAL "ubuntu-12.04")
find_package(PostgreSQLFixed)
else()
find_package(PostgreSQL)
endif()
endif()

if(WITH_SQLITE3)
if(NOT WIN32)
find_package(Sqlite3)
endif()
endif()

if(WITH_LIBHPDF)
if(NOT WIN32)
find_package(LibHpdf)
endif()
endif()

enable_testing()

add_subdirectory(3rdParty)
add_subdirectory(src)
add_subdirectory(wtest)
add_subdirectory(tests)
add_subdirectory(clients)
# TODO: later, deploy public header files for development package
#add_subdirectory(include)
add_subdirectory(examples)
  
MESSAGE("Platform:")
MESSAGE("  Host: ${CMAKE_HOST_SYSTEM_NAME} ${CMAKE_HOST_SYSTEM_VERSION} ${CMAKE_HOST_SYSTEM_PROCESSOR}")
MESSAGE("  Canonical: ${INSTALLER_PLATFORM}")
MESSAGE("  CMake: ${CMAKE_VERSION}")
MESSAGE("  CMake generator: ${CMAKE_GENERATOR}")
MESSAGE("  CMake build tool: ${CMAKE_BUILD_TOOL}")
if(MSVC)
  MESSAGE("  MSVC: ${MSVC_VERSION}")
endif()
if(NOT CMAKE_GENERATOR MATCHES "Visual Studio")
  MESSAGE("  Build Configuration: ${CMAKE_BUILD_TYPE}")
endif()

MESSAGE("Compiler:")
MESSAGE("  C++ compilation flags: ${CMAKE_CXX_FLAGS}")
MESSAGE("  C compilation flags: ${CMAKE_C_FLAGS}")

MESSAGE("Options:")

MESSAGE("  Boost: yes")
MESSAGE("    Includes: ${Boost_INCLUDE_DIRS}")
MESSAGE("    Libpath: ${Boost_LIBRARY_DIRS}")
MESSAGE("    Libraries: ${Boost_LIBRARIES}")

if(WITH_SSL)
if(OPENSSL_FOUND)
  MESSAGE("  SSL: yes")
else()
  MESSAGE("  SSL: not found")
endif()
endif()

if(WITH_TEXTWOLF)
  MESSAGE("  Textwolf: yes")
endif()

if(WITH_LUA)
  MESSAGE("  LUA: yes")
endif()

if(WITH_DOTNET)
  MESSAGE("  .NET: yes")
endif()

if(WITH_CJSON)
  MESSAGE("  CJSON: yes")
endif()

if(WITH_LIBXML2)
if(LIBXML2_FOUND)
  MESSAGE("  libxml2: yes")
else()
  MESSAGE("  libxml2: not found")
endif()
endif()

if(WITH_LIBXSLT)
if(LIBXSLT_FOUND)
  MESSAGE("  libxslt: yes")
else()
  MESSAGE("  libxslt: not found")
endif()
endif()

if(WITH_SQLITE3)
  MESSAGE("  Sqlite3: yes")
if(NOT WIN32)
  MESSAGE("    Includes: ${SQLITE3_INCLUDE_DIR}")
  MESSAGE("    Libraries: ${SQLITE3_LIBRARIES}")
endif()
endif()

if(WITH_PGSQL)
if(PostgreSQL_FOUND)
  MESSAGE("  PostgreSQL: yes")
  MESSAGE("    Includes: ${PostgreSQL_INCLUDE_DIRS}")
  MESSAGE("    Libpath: ${PostgreSQL_LIBRARY_DIRS}")
  MESSAGE("    Libraries: ${PostgreSQL_LIBRARIES}")
else()
  MESSAGE("  PostgreSQL: not found")
endif()
endif()

if(WITH_ICU)
  MESSAGE("  ICU: yes")
endif()

if(WITH_LIBHPDF)
  MESSAGE("  libhpdf: yes")
if(NOT WIN32)
  MESSAGE("    Includes: ${LIBHPDF_INCLUDE_DIRS}")
  MESSAGE("    Libraries: ${LIBHPDF_LIBRARIES}")
endif()
endif()
