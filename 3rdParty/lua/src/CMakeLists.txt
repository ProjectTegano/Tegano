cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(liblua_source_files
  lauxlib.c
  ldblib.c
  lualib.h
  lstate.h
  liolib.c
  luac.c
  lvm.c
  ldebug.h
  lapi.c
  lcode.h
  lundump.h
  lauxlib.h
  lobject.h
  lbaselib.c
  lzio.h
  lstate.c
  ldo.h
  lparser.c
  lgc.c
  ltablib.c
  lobject.c
  lundump.c
  lparser.h
  lstrlib.c
  lctype.h
  ldump.c
  lstring.h
  lctype.c
  lcode.c
  lcorolib.c
  lopcodes.c
  lvm.h
  llimits.h
  luaconf.h
  loslib.c
  lfunc.c
  lapi.h
  ltm.h
  lmem.c
  lbitlib.c
  lfunc.h
  lmathlib.c
  lmem.h
  lzio.c
  loadlib.c
  lstring.c
  llex.h
  ltable.c
  ltm.c
  ltable.h
  llex.c
  lua.h
  lgc.h
  ldo.c
  lopcodes.h
  ldebug.c
  linit.c
)

if(NOT WIN32)
  add_definitions("-fPIC")
endif()

add_library(lua STATIC ${liblua_source_files})
  
