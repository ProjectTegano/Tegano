cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(libconfig_source_files
  aaaaProviderConfiguration.cpp
  aaaaProviderConfiguration.hpp
  applicationConfiguration.cpp
  applicationConfiguration.hpp
  applicationInfo.cpp
  applicationInfo.hpp
  applicationProperties.cpp
  applicationProperties.hpp
  bannerConfiguration.cpp
  bannerConfiguration.hpp
  cmdLineConfiguration.cpp
  cmdLineConfiguration.hpp
  databaseProviderConfiguration.cpp
  databaseProviderConfiguration.hpp
  loggerConfiguration.cpp
  loggerConfiguration.hpp
  moduleConfigParseUtils.hpp
  moduleDirectoryImpl.cpp
  moduleDirectoryImpl.hpp
  moduleLoader.cpp
  moduleLoader.hpp
  procProviderConfiguration.cpp
  procProviderConfiguration.hpp
  serverConfiguration.cpp
  serverConfiguration.hpp
  serviceConfiguration.cpp
  serviceConfiguration.hpp
  serverEndpoint.cpp
  version.cpp
  version.hpp
)

if(WITH_SSL)
  add_definitions("-DWITH_SSL")
endif()

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
  ${OPENSSL_INCLUDE_DIR}
)

link_directories(${Boost_LIBRARY_DIRS})

if(WIN32)
  add_library(config STATIC ${libconfig_source_files} ${OPENSSL_LIBRARIES})
else()
  add_library(config SHARED ${libconfig_source_files} ${OPENSSL_LIBRARIES})
endif()

target_link_libraries(config ${CMAKE_DL_LIBS}
  wolframe
)
