cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

if(WITH_LIBXML2)
add_subdirectory(libxml2)
endif()

if(WITH_LIBXSLT)
add_subdirectory(libxslt)
endif()

if(WITH_LIBHPDF)
#add_subdirectory(libhpdf)
endif()
