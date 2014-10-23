cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(zlib_source_files
  zutil.h
  gzguts.h
  gzlib.c
  inftrees.h
  inffast.c
  inffast.h
  trees.h
  gzclose.c
  compress.c
  zconf.h
  trees.c
  adler32.c
  inflate.h
  inffixed.h
  inftrees.c
  uncompr.c
  gzread.c
  deflate.c
  zutil.c
  zlib.h
  infback.c
  deflate.h
  crc32.h
  inflate.c
  crc32.c
  gzwrite.c
)

add_library(zlib STATIC ${zlib_source_files})
