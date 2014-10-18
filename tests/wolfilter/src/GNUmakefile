TOPDIR = ../../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR) \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	-I$(TOPDIR)/src/wolfilter \
	-I$(TOPDIR)/3rdParty/gtest/include \
	-I$(TOPDIR)/wtest/include \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/src/libwfsingleton \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_serialize \
	-L$(TOPDIR)/src/libwolframe_database \
	-L$(TOPDIR)/src/libwolframe_langbind \
	-L$(TOPDIR)/src/libconfig \
	-L$(TOPDIR)/src/libprovider \
	-L$(TOPDIR)/wtest

INCLUDE_LIBS = \
	-lwfconfig \
	-lwfprovider \
	-lwfsingleton \
	-lwolftest \
	-lwolframe \
	-lwolframe_serialize \
	-lwolframe_database \
	-lwolframe_langbind \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_program_options \
	-lboost_filesystem \
	$(TOPDIR)/3rdParty/gtest/gtest-all.o \
	$(LIBS_DL) $(LIBS_LT) $(LIBS_NET)

# needed because libwtest depends on database functions

# postgres
ifeq ($(WITH_PGSQL),1)
INCLUDE_CXXFLAGS += \
	-DWITH_PGSQL
INCLUDE_DIRS += \
	$(PGSQL_INCLUDE_DIRS)
INCLUDE_LDFLAGS += \
	$(PGSQL_LIB_DIRS)
INCLUDE_LIBS += \
	$(PGSQL_LIBS)
endif

# Oracle
ifeq ($(WITH_ORACLE),1)
INCLUDE_CXXFLAGS += \
	-DWITH_ORACLE
INCLUDE_DIRS += \
	$(ORACLE_INCLUDE_DIRS)
INCLUDE_LDFLAGS += \
	$(ORACLE_LIB_DIRS)
INCLUDE_LIBS += \
	$(ORACLE_LIBS)
endif


CPP_OBJS = \
	testWolfilterFiles.o
	
CPP_BINS = \
	testWolfilter$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

testWolfilter$(EXE): testWolfilter.o $(OBJS) $(CPP_OBJS)
	$(CXX_LINK) -o $@ $(ALL_LDFLAGS) $(OBJS) $(CPP_OBJS) $< $(LIBS)

local_all:

local_clean:
	-rm -f $(CPP_BINS)
	-rm -f $(CPP_OBJS)

local_distclean:

local_install:

local_uninstall:

LD_LIBRARY_PATH = "$(BOOST_LIB_DIR):$(TOPDIR)/src/libwfsingleton:$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_serialize:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/src/libwolframe_database:$(TOPDIR)/wtest"

ifeq ($(WITH_LIBXML2),1)
LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(LIBXML2_LIB_DIR)"
endif

ifeq ($(WITH_LIBXSLT),1)
LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(LIBXSLT_LIB_DIR)"
endif

ifeq ($(WITH_PGSQL),1)
LD_LIBRARY_PATH:= "$(LD_LIBRARY_PATH):$(PGSQL_LIB_DIR)"
endif

ifeq ($(WITH_ORACLE),1)
LD_LIBRARY_PATH:= "$(LD_LIBRARY_PATH):$(ORACLE_LIB_DIR)"
endif


ifeq ($(WITH_LIBHPDF),1)
LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(LIBHPDF_LIB_DIR)"
endif

local_test: $(CPP_BINS)
ifeq ($(RUN_TESTS),1)
	@mkdir -p temp
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./testWolfilter
	@echo
endif

