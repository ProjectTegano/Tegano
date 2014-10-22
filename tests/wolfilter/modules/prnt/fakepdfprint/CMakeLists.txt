cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(mod_print_testpdf_source_files
  mod_print_testpdf.cpp
  pdfPrinterDocumentImpl.cpp
  pdfPrinterDocumentImpl.hpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/src/modules/prnt/harupdfprint"
)

add_library(mod_print_testpdf MODULE ${mod_print_testpdf_source_files})

set_target_properties(mod_print_testpdf PROPERTIES PREFIX "")

target_link_libraries(mod_print_testpdf print_harupdf_static)
