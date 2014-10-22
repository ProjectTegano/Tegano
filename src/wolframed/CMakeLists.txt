cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(wolframed_source_files
  connection.cpp
  serverConnectionHandler.hpp
  wolframedCommandLine.cpp
  mainConnectionHandler.hpp
  acceptor.hpp
  mainConnectionHandler.cpp
  connectionBase.hpp
  serverHandler.hpp
  connection.hpp
  serverHandler.cpp
  wolframed.cpp
  serverConnectionHandler.cpp
  wolframedCommandLine.hpp
  server.hpp
  server.cpp
  acceptor.cpp
  errorCode.hpp
)

if(WIN32)
  set(wolframed_source_files ${wolframed_source_files}
    winMain.cpp
  )
else()
  set(wolframed_source_files ${wolframed_source_files}
    posixMain.cpp
  )  
endif()

if(WITH_SSL)
  add_definitions("-DWITH_SSL")
  set(wolframed_source_files ${wolframed_source_files}
    SSLcertificateInfo.cpp
  )
  set(ssl_libraries "${OPENSSL_LIBRARIES}")  
endif()

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/src"
  .
)

add_executable(wolframed ${wolframed_source_files})

target_link_libraries(wolframed ${Boost_LIBRARIES} pthread ${CMAKE_DL_LIBS} ${ssl_libraries}
  config wfsingleton wolframe provider
  wolframe_langbind
)
