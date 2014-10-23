cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(testlibhpdf_source_files
  testlibhpdf.cpp
)

include_directories(
  ${LIBHPDF_INCLUDE_DIR}
)

add_executable(testlibhpdf ${testlibhpdf_source_files})

target_link_libraries(testlibhpdf ${LIBHPDF_LIBRARIES})

add_test(NAME testlibhpdf COMMAND testlibhpdf)
