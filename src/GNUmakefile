TOPDIR = ..

SUBDIRS = \
	libwolframe \
	libwfsingleton \
	libwolframe_serialize \
	libwolframe_database \
	libwolframe_langbind \
	libconfig \
	libprovider \
	modules \
	wolframed \
	wolfilter \
	wolfwizard

-include $(TOPDIR)/makefiles/gmake/platform.mk

-include $(TOPDIR)/makefiles/gmake/sub.mk

libwfsingleton: libwolframe
modules: libwfsingleton libwolframe libwolframe_serialize libwolframe_langbind
libwolframe_serialize: libwolframe
libwolframe_langbind: libwolframe libwolframe_serialize libwolframe_database
libwolframe_database: libwfsingleton libwolframe
wolframed: libwfsingleton libwolframe libwolframe_serialize libwolframe_database libwolframe_langbind libconfig libprovider
wolfilter: libwfsingleton libwolframe libwolframe_serialize libwolframe_database libwolframe_langbind libconfig libprovider
wolfwizard: libwfsingleton libwolframe libwolframe_serialize libwolframe_database libwolframe_langbind libconfig libprovider

local_all:

local_clean:

local_distclean:

local_test:

