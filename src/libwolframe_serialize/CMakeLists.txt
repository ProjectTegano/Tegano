cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(libwolframe_serialize_source_files
  ddl/ddlStructSerializer.cpp
  ddl/ddlStructParser.cpp
  cppFormFunction.cpp
  struct/parseStack.cpp
  struct/serializeStack.cpp
  struct/structDescriptionBase.cpp
  struct/structSerializer.cpp
  struct/intrusiveSerializer.cpp
  struct/structParser.cpp
  struct/intrusiveParser.cpp
  serializationErrorException.hpp
  structOptionParser.cpp
  tostringUtils.cpp
  serializationErrorException.cpp
  configSerialize.cpp
)

include_directories(
  "${Boost_INCLUDE_DIRS}"
  "${PROJECT_SOURCE_DIR}/include"
  .
)

add_library(wolframe_serialize SHARED ${libwolframe_serialize_source_files})
