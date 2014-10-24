cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(TESTS
  versionTest
  logTest 
  testMethodMemberPointerTemplate 
  testNamedRetVal
  testVirtualInheritance
  resolvePathTest
  testBoostPath
  testFiletypeRecognition
  byte2hexTest
  base64Test
  testRand
  testTimestamp
  testVariantTypes
  testVariantStruct
  configValueParser
  configStructParser
  testIntegers
  testNumberBaseConversion
  testString
  testDateTime
  testBigNumber
)

if(LONG_TESTS)
set(TESTS ${TESTS}
  testAddressRestriction
  testArrayDoublingAllocator
  poolTest
)
endif()

# only for memory corruption checks, doesn't compile currently): testTestAllocator
# not a test, should maybe go to the library: base64Utility

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/3rdParty/gtest/include"
  "${PROJECT_SOURCE_DIR}/wtest/include"
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/src"
)

link_directories(${Boost_LIBRARY_DIRS})

foreach(test ${TESTS})
  add_executable(${test} ${test}.cpp)
  target_link_libraries(${test}
    gtest ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES} ${CMAKE_DL_LIBS}
    config wfsingleton wolframe provider
    wolframe_langbind wolframe_serialize
  )
  add_test(NAME ${test} COMMAND ${test})
endforeach(test)

add_subdirectory(3rdParty)
add_subdirectory(crypto)
add_subdirectory(modules)
#add_subdirectory(moduleloader)
add_subdirectory(database)
add_subdirectory(authentication)
add_subdirectory(simpleFormCompiler)
add_subdirectory(wolfilter)
add_subdirectory(compileTDL)
add_subdirectory(protocol)
add_subdirectory(doxygen_examples)
