TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

ifeq ($(WITH_LIBXML2),1)
SUBDIRS += libxml2
endif

ifeq ($(WITH_LIBXSLT),1)
SUBDIRS += libxslt
endif

ifeq ($(WITH_LIBHPDF),1)
SUBDIRS += libhpdf
endif

-include $(TOPDIR)/makefiles/gmake/sub.mk
