# from http://thomasfischer.biz/find-out-the-linux-distribution-and-version-in-cmake/

if(NOT WIN32)
# use the LSB stuff if possible :)
EXECUTE_PROCESS(
  COMMAND cat /etc/lsb-release
  COMMAND grep DISTRIB_ID
  COMMAND awk -F= "{ print $2 }"
  COMMAND tr "\n" " "
  COMMAND sed "s/ //"
  OUTPUT_VARIABLE LSB_ID
  RESULT_VARIABLE LSB_ID_RESULT
)
EXECUTE_PROCESS(
  COMMAND cat /etc/lsb-release
  COMMAND grep DISTRIB_RELEASE
  COMMAND awk -F= "{ print $2 }"
  COMMAND tr "\n" " "
  COMMAND sed "s/ //"
  OUTPUT_VARIABLE LSB_VER
  RESULT_VARIABLE LSB_VER_RESULT
)

message("LSB output: ${LSB_ID_RESULT}:${LSB_ID} ${LSB_VER_RESULT}:${LSB_VER}")

if(NOT ${LSB_ID} STREQUAL "")
  # found some, use it :D
  set(INSTALLER_PLATFORM "${LSB_ID}-${LSB_VER}" CACHE PATH "Installer chosen platform")
else(NOT ${LSB_ID} STREQUAL "")
  set(INSTALLER_PLATFORM "linux-generic" CACHE PATH "Installer chosen platform")
endif(NOT ${LSB_ID} STREQUAL "")

else(NOT WIN32)
set(INSTALLER_PLATFORM "windows-generic" CACHE PATH "Installer chosen platform")
endif(NOT WIN32)
