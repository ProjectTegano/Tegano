cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(testlibhpdf_source_files
  testlibhpdf.cpp
)

if(WIN32)
  set(hpdf_includes "${PROJECT_SOURCE_DIR}/3rdParty/libhpdf/include")
else()
  set(hpdf_includes ${LIBHPDF_INCLUDE_DIRS})
endif()

include_directories(
  ${hpdf_includes}
)

link_directories(${Boost_LIBRARY_DIRS})

add_executable(testlibhpdf ${testlibhpdf_source_files})

if(WIN32)
  set(hpdf_libraries hpdf zlib png)
else()
  set(hpdf_libraries ${LIBHPDF_LIBRARIES})
endif()

target_link_libraries(testlibhpdf ${hpdf_libraries})

add_test(NAME testlibhpdf COMMAND testlibhpdf)
