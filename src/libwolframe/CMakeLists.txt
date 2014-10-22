cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(libwolframe_source_files
  processor/execContext.cpp
  system/platform.cpp
  logger/logError.cpp
  utils/sourceLineInfo.cpp
  utils/printFormats.cpp
  utils/fileLineInfo.cpp
  utils/asciiCharParser.cpp
  utils/malloc.cpp
  utils/getPassword.cpp
  utils/parseUtils.cpp
  utils/fileUtils.cpp
  utils/stringUtils.cpp
  utils/allocators.cpp
  aaaa/user.cpp
  aaaa/passwordChangeMessage.cpp
  aaaa/usernameHash.cpp
  aaaa/CRAM.cpp
  aaaa/passwordHash.cpp
  crypto/hmac_sha1.c
  crypto/pbkdf2_hmac_sha1.c
  crypto/md5.c
  crypto/AES256.c
  crypto/sha1.c
  crypto/sha2.c
  crypto/hmac_sha256.c
  types/b64encode.c
  types/b64decode.c
  types/byte2hex.c
  crypto/HMAC.cpp
  crypto/PBKDF2.cpp
  types/docmetadata.cpp
  types/propertyTree.cpp
  types/customDataType.cpp
  types/addressRestriction.cpp
  types/objectPool.cpp
  types/datetime.cpp
  types/variantStructDescription.cpp
  types/variant.cpp
  types/conversions.cpp
  types/string.cpp
  types/bignumber.cpp
  types/form.cpp
  types/numberBaseConversion.cpp
  types/variantStruct.cpp
  types/base64.cpp
)

if(WITH_TEXTWOLF)
  add_definitions("-DWITH_TEXTWOLF")
  set(textwolf_includes "${PROJECT_SOURCE_DIR}/3rdParty/textwolf/include")
endif()

include_directories(
  ${Boost_INCLUDE_DIRS}
  "${PROJECT_SOURCE_DIR}/include"
  .
  ${textwolf_includes}
)

add_library(wolframe SHARED ${libwolframe_source_files})

target_link_libraries(wolframe
  ${Boost_LIBRARIES}
)
