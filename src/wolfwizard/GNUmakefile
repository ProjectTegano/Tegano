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

CPP_OBJS = \
	wolfwizardCommandLine.o

CPP_BINS = \
	wolfwizard$(EXE)

-include $(TOPDIR)/makefiles/gmake/sub.mk

wolfwizard$(EXE): wolfwizard.o $(OBJS) $(CPP_OBJS)
	$(CXX_LINK) -o $@ $(ALL_LDFLAGS) $(OBJS) $(CPP_OBJS) $< $(LIBS)

local_all:

local_clean:

local_distclean:

local_install:
	$(INSTALL) -d -m 0755 $(DESTDIR)$(bindir)
	$(INSTALL) -m 0775 wolfwizard$(EXE) $(DESTDIR)$(bindir)

local_uninstall:
	@-rm -f $(DESTDIR)$(bindir)/wolfwizard$(EXE)
	@-rmdir $(DESTDIR)$(bindir)

local_test:

