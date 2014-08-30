TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_CXXFLAGS =

INCLUDE_DIRS = \
	-I. \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/src \
	-I$(BOOST_INCLUDE_DIR)

INCLUDE_LDFLAGS = \
	-L$(BOOST_LIB_DIR) \
	-L$(TOPDIR)/src/libwfsingleton \
	-L$(TOPDIR)/src/libwolframe \
	-L$(TOPDIR)/src/libwolframe_serialize \
	-L$(TOPDIR)/src/libwolframe_database \
	-L$(TOPDIR)/src/libwolframe_langbind \
	-L$(TOPDIR)/src/libconfig \
	-L$(TOPDIR)/src/libprovider

INCLUDE_LIBS = \
	-lwfconfig \
	-lwfprovider \
	-lwfsingleton \
	-lwolframe \
	-lwolframe_serialize \
	-lwolframe_database \
	-lwolframe_langbind \
	-lboost_thread$(BOOST_LIBRARY_TAG) -lboost_system \
	-lboost_program_options \
	-lboost_filesystem \
	$(LIBS_DL) $(LIBS_LT) $(LIBS_NET)

# openssl
ifeq ($(WITH_SSL),1)

INCLUDE_CXXFLAGS += \
	-DWITH_SSL

INCLUDE_DIRS += \
	$(OPENSSL_INCLUDE_DIRS)
	
INCLUDE_LDFLAGS += \
	$(OPENSSL_LIB_DIRS)

INCLUDE_LIBS += \
	$(OPENSSL_LIBS)
endif

CPP_OBJS = \
	acceptor.o \
	connection.o \
	mainConnectionHandler.o \
	serverConnectionHandler.o \
	server.o \
	serverEndpoint.o \
	serverHandler.o \
	posixMain.o \
	wolframedCommandLine.o

#SSL
ifeq ($(WITH_SSL),1)
CPP_OBJS += \
	SSLcertificateInfo.o
endif

CPP_BINS = \
	wolframed$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

wolframed$(EXE): wolframed.o $(OBJS) $(CPP_OBJS)
	$(CXX_LINK) -o $@ $(ALL_LDFLAGS) $(OBJS) $(CPP_OBJS) $< $(LIBS)

local_all:

local_clean:
	@rm -f primary.db

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(sbindir)
	$(INSTALL) -m 0775 wolframed$(EXE) $(DESTDIR)$(sbindir)

local_uninstall:
	@-rm -f $(DESTDIR)$(sbindir)/wolframed$(EXE)
	@-rmdir $(DESTDIR)$(sbindir)

local_test:

LD_LIBRARY_PATH = "$(BOOST_LIB_DIR):$(TOPDIR)/src/libwfsingleton/:$(TOPDIR)/src/libwolframe:$(TOPDIR)/src/libwolframe_serialize:$(TOPDIR)/src/libwolframe_langbind:$(TOPDIR)/src/libwolframe_database"

ifeq ($(WITH_ICU),1)
LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(ICU_LIB_DIR)"
endif

ifeq ($(WITH_PGSQL),1)
LD_LIBRARY_PATH:= "$(LD_LIBRARY_PATH):$(PGSQL_LIB_DIR)"
endif

ifeq ($(WITH_LIBXML2),1)
LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(LIBXML2_LIB_DIR)"
endif

ifeq ($(WITH_LIBHPDF),1)
LD_LIBRARY_PATH := "$(LD_LIBRARY_PATH):$(LIBHPDF_LIB_DIR)"
endif

run:
	@echo "Running the Wolframe server..."
	@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./wolframed -d DATA -c wolframe.conf -f

check:
	@echo "Running the Wolframe server..."
	@@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./wolframed -c wolframe.conf -t

print:
	@echo "Running the Wolframe server..."
	@@LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./wolframed -c wolframe.conf -p

showenv:
	@echo LD_LIBRARY_PATH=$(LD_LIBRARY_PATH)
