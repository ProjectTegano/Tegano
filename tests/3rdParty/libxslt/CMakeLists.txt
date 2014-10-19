cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(testlibxslt1_source_files
  testlibxslt1.cpp
)

add_definitions("-DWITH_LIBXML2")

if(WITH_LIBXSLT)
  add_definitions("-DWITH_LIBXSLT")
endif()

include_directories(
  "${LIBXML2_INCLUDE_DIR}"
  "${LIBXSLT_INCLUDE_DIR}"
)

add_executable(testlibxslt1 ${testlibxslt1_source_files})

target_link_libraries(testlibxslt1 ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES})

#TODO: must compare correctnes inside the C binary!
add_test(NAME libxslt1 COMMAND testlibxslt1 test1.xslt test1.xml)
