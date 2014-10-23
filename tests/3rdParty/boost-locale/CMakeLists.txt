cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(boostLocalTest_source_files
  boostLocaleTest.cpp
)

include_directories(
)

add_executable(boostLocalTest ${boostLocalTest_source_files})

target_link_libraries(boostLocalTest)

# TODO: setenv LANG en_US.utf8
#add_test(NAME boostLocalTest COMMAND LANG=en.US-UTF-8 boostLocalTest)

target_link_libraries(boostLocalTest gtest
  ${Boost_LIBRARIES}
)
