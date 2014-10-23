cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(libgtest_source_files
 src/gtest-all.cc
)

include_directories(include .)

add_library(gtest STATIC ${libgtest_source_files})

target_link_libraries(gtest ${CMAKE_THREAD_LIBS_INIT})
