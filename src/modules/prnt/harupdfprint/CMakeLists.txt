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
  "${Boost_INCLUDE_DIRS}"
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/3rdParty/textwolf/include"
)

add_library(mod_print_harupdf MODULE ${mod_print_harupdf_source_files})

set_target_properties(mod_print_harupdf PROPERTIES PREFIX "")

target_link_libraries(mod_print_harupdf
)
#  wolframe_database wolframe_serialize
#  ${SQLITE3_LIBRARIES}
#)

add_definitions("-fPIC")

add_library(print_harupdf_static STATIC ${print_harupdf_source_files})

target_link_libraries(print_harupdf_static
)
# ${CMAKE_DL_LIBS}
#  sqlite3
#  wolframe_database wolframe_serialize
#)
