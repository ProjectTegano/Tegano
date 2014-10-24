cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(passwd_source_files
  passwdFile.hpp
  passwdFile.cpp
  wolfpasswdFiles.cpp
  wolfpasswd.cpp
)

set(auth_textfile_source_files
  TextFileAuth.hpp
  TextFileAuth.cpp
  TextFileAuthConfig.cpp
  ${passwd_source_files}
)

set(mod_auth_textfile_source_files
  ${auth_textfile_source_files}
  mod_auth_textfile.cpp
)

set(wolfpasswd_source_file
  wolfpasswd.cpp
  ${passwd_source_files}
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/src"
)

link_directories(${Boost_LIBRARY_DIRS})

add_library(mod_auth_textfile MODULE ${mod_auth_textfile_source_files})

set_target_properties(mod_auth_textfile PROPERTIES PREFIX "")

target_link_libraries(mod_auth_textfile
  wfsingleton wolframe
)

add_library(auth_textfile_static STATIC ${auth_textfile_source_files})

add_executable(wolfpasswd ${wolfpasswd_source_file})

target_link_libraries(wolfpasswd 
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_LIBRARIES} 
  wfsingleton wolframe
)
