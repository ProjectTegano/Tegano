cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(libprovider_source_files
  aaaaProviderImpl.cpp
  aaaaProviderImpl.hpp
  auditProvider.cpp
  auditProvider.hpp
  authenticationFactory.cpp
  authenticationFactory.hpp
  authorizationProvider.cpp
  authorizationProvider.hpp
  databaseProviderImpl.cpp
  databaseProviderImpl.hpp
  ddlProgram.cpp
  ddlProgram.hpp
  normalizeProgram.cpp
  normalizeProgram.hpp
  procProviderImpl.cpp
  procProviderImpl.hpp
  programLibraryImpl.cpp
  programLibraryImpl.hpp
  providerUtils.hpp
  randomGeneratorImpl.cpp
  randomGeneratorImpl.hpp
  standardAudit.cpp
  standardAudit.hpp
  standardAuthenticator.cpp
  standardAuthenticator.hpp
  standardAuthorizer.cpp
  standardAuthorizer.hpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
)

link_directories(${Boost_LIBRARY_DIRS})

if(WIN32)
  add_library(provider STATIC ${libprovider_source_files})
else()
  add_library(provider SHARED ${libprovider_source_files})
endif()

target_link_libraries(provider wfsingleton)
