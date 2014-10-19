cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(testlibxml2_source_files
  testlibxml2.cpp
)

include_directories(
  "${LIBXML2_INCLUDE_DIR}"
)

add_executable(testlibxml2 ${testlibxml2_source_files})

target_link_libraries(testlibxml2 ${LIBXML2_LIBRARIES})

#TODO: must write RES files inside the test and compare against
# the MUST files there!
add_test(NAME libxml2-UTF-8 COMMAND testlibxml2 test-UTF-8.xml)
add_test(NAME libxml2-UTF-16LE COMMAND testlibxml2 test-UTF-16LE.xml)
add_test(NAME libxml2-UTF-16BE COMMAND testlibxml2 test-UTF-16BE.xml)
add_test(NAME libxml2-UCS-2BE COMMAND testlibxml2 test-UCS-2BE.xml)
add_test(NAME libxml2-UCS-2LE COMMAND testlibxml2 test-UCS-2LE.xml)
