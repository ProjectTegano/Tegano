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
  ${print_harupdf_source_files}
)

if(WIN32)
  set(hpdf_includes "${PROJECT_SOURCE_DIR}/3rdParty/libhpdf/include")
else()
  set(hpdf_includes ${LIBHPDF_INCLUDE_DIRS})
endif()

include_directories(
  ${hpdf_includes}
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/3rdParty/textwolf/include"
)

link_directories(${Boost_LIBRARY_DIRS})

add_library(mod_print_harupdf MODULE ${mod_print_harupdf_source_files})

set_target_properties(mod_print_harupdf PROPERTIES PREFIX "")

if(WIN32)
  set(hpdf_libraries hpdf zlib png)
else()
  set(hpdf_libraries ${LIBHPDF_LIBRARIES})
endif()

target_link_libraries(mod_print_harupdf
  wfsingleton
  wolframe
  ${hpdf_libraries}
)

if(NOT WIN32)
  add_definitions("-fPIC")
endif()

add_library(print_harupdf_static STATIC ${print_harupdf_source_files})

target_link_libraries(print_harupdf_static
  wfsingleton
  wolframe
  ${hpdf_libraries}
)
