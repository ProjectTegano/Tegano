TOPDIR = ../..

SUBDIRS =

PACKAGE_NAME = wolframe

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

# textwolf support for module types/string
ifeq ($(WITH_TEXTWOLF),1)
INCLUDE_CXXFLAGS += \
	-DWITH_TEXTWOLF
endif

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/src/libwolframe

INCLUDE_DIRS = \
	-I. -I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

# textwolf support for module types/string
ifeq ($(WITH_TEXTWOLF),1)
INCLUDE_DIRS += \
	-I$(TOPDIR)/3rdParty/textwolf/include
endif

INCLUDE_LIBS = \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_filesystem \

STATIC_LIB = libwolframe.a

DYNAMIC_LIB = libwolframe.so
DYNAMIC_LIB_MAJOR = 0
DYNAMIC_LIB_MINOR = 0
DYNAMIC_LIB_PATCH = 0

TYP_OBJS = \
	types/base64.o \
	types/b64encode.o \
	types/b64decode.o \
	types/objectPool.o \
	types/docmetadata.o \
	types/conversions.o \
	types/numberBaseConversion.o \
	types/propertyTree.o \
	types/string.o \
	types/bignumber.o \
	types/datetime.o \
	types/customDataType.o \
	types/addressRestriction.o \
	types/variant.o \
	types/variantStructDescription.o \
	types/variantStruct.o \
	types/form.o

UTIL_OBJS = \
	utils/asciiCharParser.o \
	utils/fileLineInfo.o \
	utils/sourceLineInfo.o \
	utils/allocators.o \
	utils/malloc.o \
	utils/stringUtils.o \
	utils/fileUtils.o \
	utils/parseUtils.o \
	utils/printFormats.o \
	utils/getPassword.o

AAAA_OBJS = \
	aaaa/user.o \
	aaaa/passwordHash.o \
	aaaa/usernameHash.o \
	aaaa/CRAM.o \
	aaaa/passwordChangeMessage.o

CPP_OBJS = \
	$(TYP_OBJS) \
	$(UTIL_OBJS) \
	$(AAAA_OBJS) \
	logger/logError.o \
	processor/execContext.o \
	system/platform.o \
	crypto/HMAC.o \
	crypto/PBKDF2.o

OBJS = \
	types/byte2hex.o \
	crypto/AES256.o \
	crypto/sha1.o \
	crypto/sha2.o \
	crypto/hmac_sha1.o \
	crypto/hmac_sha256.o \
	crypto/pbkdf2_hmac_sha1.o \
	crypto/md5.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

local_all:

local_clean:

local_distclean:

local_install:

local_test:
