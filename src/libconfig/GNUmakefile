TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/src/libwfsingleton \
	-L$(TOPDIR)/src/libwolframe

INCLUDE_LIBS = \
	-lwfsingleton \
	-lwolframe \
	-lboost_system \
	-lboost_filesystem \
	$(LIBS_DL) $(LIBS_LT) $(LIBS_NET)

# openssl
ifeq ($(WITH_SSL),1)
INCLUDE_CXXFLAGS += \
	-DWITH_SSL
endif

LOCAL_STATIC_LIB = \
	libwfconfig.a

LOCAL_STATIC_LIB_OBJS = \
	version.o \
	moduleDirectoryImpl.o \
	moduleLoader.o \
	serviceConfiguration.o \
	loggerConfiguration.o \
	bannerConfiguration.o \
	serverConfiguration.o \
	cmdLineConfiguration.o \
	procProviderConfiguration.o \
	AAAAproviderConfiguration.o \
	applicationInfo.o \
	applicationConfiguration.o \
	applicationProperties.o

CPP_OBJS = \
	$(LOCAL_STATIC_LIB_OBJS)

CPP_BINS =

-include $(TOPDIR)/makefiles/gmake/sub.mk

# set default module load directory for configuration option 'directory' in 'LoadModules'
ifdef DEFAULT_MODULE_LOAD_DIR
INCLUDE_CXXFLAGS += -DDEFAULT_MODULE_LOAD_DIR=$(DEFAULT_MODULE_LOAD_DIR)
endif

# set default main configuration file location
ifdef DEFAULT_MAIN_CONFIGURATION_FILE
INCLUDE_CXXFLAGS += -DDEFAULT_MAIN_CONFIGURATION_FILE=$(DEFAULT_MAIN_CONFIGURATION_FILE)
endif

local_all: $(LOCAL_STATIC_LIB)

$(LOCAL_STATIC_LIB): $(LOCAL_STATIC_LIB_OBJS)
	ar rcs $(LOCAL_STATIC_LIB) $(LOCAL_STATIC_LIB_OBJS)

local_clean:
	@rm -f $(LOCAL_STATIC_LIB)

local_distclean:

local_install:

local_uninstall:

local_test:

