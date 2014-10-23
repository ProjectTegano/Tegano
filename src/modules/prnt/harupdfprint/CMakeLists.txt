cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(print_harupdf_source_files
  pdfPrinterExpression.cpp
  pdfPrinterVariable.hpp
  pdfPrintProgramType.hpp
  pdfPrinter.cpp
  pdfPrinter.hpp
  pdfPrinterDocumentImpl.cpp
  pdfPrinterDocumentImpl.hpp
  pdfPrinterVariable.cpp
  pdfPrinterMethod.hpp
  pdfPrinterExpression.hpp
  pdfPrinterDocument.hpp
  pdfPrinterMethod.cpp
  pdfPrintProgramType.cpp
)

set(mod_print_harupdf_source_files
  mod_print_harupdf.cpp
  ${mod_print_harupdf_source_files}
)

include_directories(
  ${LIBHPDF_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/3rdParty/textwolf/include"
)

add_library(mod_print_harupdf MODULE ${mod_print_harupdf_source_files})

set_target_properties(mod_print_harupdf PROPERTIES PREFIX "")

target_link_libraries(mod_print_harupdf
)

if(NOT WIN32)
  add_definitions("-fPIC")
endif()

add_library(print_harupdf_static STATIC ${print_harupdf_source_files})
