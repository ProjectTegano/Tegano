cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(libpng_source_files
  pngset.c
  pngwutil.c
  pngconf.h
  pngwtran.c
  png.h
  pngpriv.h
  pngget.c
  pngrutil.c
  png.c
  pngrio.c
  pngpread.c
  pngrtran.c
  pnginfo.h
  pngmem.c
  pngstruct.h
  pngtrans.c
  pngerror.c
  pngwio.c
  pngdebug.h
  pngtest.c
  pngwrite.c
  pnglibconf.h
  pngread.c
)

add_library(png STATIC ${libpng_source_files})
