cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(wolframec_source_files
  utils.hpp
  options.hpp
  utils.cpp
  options.cpp
  wolframe.hpp
  wolframec.cpp
)

if(WITH_SSL)
  add_definitions("-DWITH_SSL")
  set(ssl_libraries "${OPENSSL_LIBRARIES}")  
endif()

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/clients/lib"
)

link_directories(${Boost_LIBRARY_DIRS})

add_executable(wolframec ${wolframec_source_files})

target_link_libraries(wolframec ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${ssl_libraries}
  wolframe_client
)
