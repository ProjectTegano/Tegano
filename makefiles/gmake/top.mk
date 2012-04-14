# top-level makefile for a package
#
# requires:
# - TOPDIR
# - SUBDIRS
#
# provides:
# - target 'all'
# - target 'clean'
# - target 'distclean'
# - target 'test'
# - target 'longtest'
# - target 'doc'
# - target 'dist'
# - target 'help'
# - target 'install'
# - target 'uninstall'
# - target 'config'

-include makefiles/gmake/platform.mk

.PHONY: all
all:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d all || exit 1); done)

.PHONY: clean
clean:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d clean || exit 1); done)
	@-test ! -f $(TOPDIR)/makefiles/gmake/platform.mk.vars && rm $(TOPDIR)/makefiles/gmake/platform.mk.vars
	@-test ! -f $(TOPDIR)/makefiles/gmake/platform.vars || rm $(TOPDIR)/makefiles/gmake/platform.vars

.PHONY: distclean
distclean:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d distclean || exit 1); done)
	@-test ! -f  $(TOPDIR)/makefiles/gmake/platform.mk.vars || rm $(TOPDIR)/makefiles/gmake/platform.mk.vars
	@-test ! -f $(TOPDIR)/makefiles/gmake/platform.vars || rm $(TOPDIR)/makefiles/gmake/platform.vars

.PHONY: install
install:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d install || exit 1); done)

.PHONY: uninstall
uninstall:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d uninstall || exit 1); done)

.PHONY: test
test: all
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d test || exit 1); done)

.PHONY: longtest
longtest: test
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d longtest || exit 1); done)

.PHONY: help
help:
	@cat $(TOPDIR)/makefiles/gmake/help.mk

.PHONY: config
config:
	@echo "Build Configuration for Wolframe"
	@echo "--------------------------------"
	@echo
	@echo "Operating system: $(PLATFORM), $(OS_MAJOR_VERSION).$(OS_MINOR_VERSION)"
	@echo "Architecture: $(ARCH)"
	@echo "System library directory: $(LIBDIR)"
ifeq "$(PLATFORM)" "LINUX"
	@echo "Linux distribution: $(LINUX_DIST) $(LINUX_REV)"
endif
	@echo "C++ Compiler: $(COMPILER)"
	@echo
	@echo "Required Dependencies:"
	@echo
	@echo "BOOST_DIR: $(BOOST_DIR)"
	@echo "BOOST_INCLUDE_DIR: $(BOOST_INCLUDE_DIR)"
	@echo "BOOST_LIB_DIR: $(BOOST_LIB_DIR)"
	@echo "BOOST_LIBRARY_TAG: $(BOOST_LIBRARY_TAG)"
	@echo
	@echo "Optional Dependencies:"
ifeq ($(WITH_SSL),1)
	@echo
	@echo "OPENSSL_DIR: $(OPENSSL_DIR)"
	@echo "OPENSSL_LIBS: $(OPENSSL_LIBS)"
endif
ifeq ($(WITH_LUA),1)
	@echo
	@echo "LUA_PLATFORM_CFLAGS: $(LUA_PLATFORM_CFLAGS)"
	@echo "LUA_PLATFORM_LDFLAGS: $(LUA_PLATFORM_LDFLAGS)"
	@echo "LUA_PLATFORM_LIBS: $(LUA_PLATFORM_LIBS)"
endif
ifeq ($(WITH_QT),1)
	@echo
	@echo "QT_DIR: $(QT_DIR)"
	@echo "QT_INCLUDE_DIR: $(QT_INCLUDE_DIR)"
	@echo "QT_LIB_DIR: $(QT_LIB_DIR)"
	@echo "QT_MOC: $(QT_MOC)"
endif
ifeq ($(WITH_PAM),1)
	@echo
	@echo "PAM_DIR: $(PAM_DIR)"
	@echo "PAM_INCLUDE_DIR: $(PAM_INCLUDE_DIR)"
	@echo "PAM_LIB_DIR: $(PAM_LIB_DIR)"
	@echo "PAM_LIBS: $(PAM_LIBS)"
endif
ifeq ($(WITH_SASL),1)
	@echo
	@echo "SASL_DIR: $(SASL_DIR)"
	@echo "SASL_INCLUDE_DIR: $(SASL_INCLUDE_DIR)"
	@echo "SASL_LIB_DIR: $(SASL_LIB_DIR)"
	@echo "SASL_LIBS: $(SASL_LIBS)"
endif
ifeq ($(WITH_SQLITE3),1)
ifeq ($(WITH_SYSTEM_SQLITE3),1)
	@echo
	@echo "SQLITE3_DIR: $(SQLITE3_DIR)"
	@echo "SQLITE3_INCLUDE_DIR: $(SQLITE3_INCLUDE_DIR)"
	@echo "SQLITE3_LIB_DIR: $(SQLITE3_LIB_DIR)"
	@echo "SQLITE3_LIBS: $(SQLITE3_LIBS)"
endif
ifeq ($(WITH_LOCAL_SQLITE3),1)
	@echo
	@echo "using local Sqlite3 amalgamation sources.."
endif
endif
ifeq ($(WITH_PGSQL),1)
	@echo
	@echo "PGSQL_DIR: $(PGSQL_DIR)"
	@echo "PGSQL_INCLUDE_DIRS: $(PGSQL_INCLUDE_DIRS)"
	@echo "PGSQL_LIB_DIRS: $(PGSQL_LIB_DIRS)"
	@echo "PGSQL_LIBS: $(PGSQL_LIBS)"
endif
ifeq ($(WITH_LIBXML2),1)
	@echo
	@echo "LIBXML2_DIR: $(LIBXML2_DIR)"
	@echo "LIBXML2_INCLUDE_DIRS: $(LIBXML2_INCLUDE_DIRS)"
	@echo "LIBXML2_LIB_DIRS: $(LIBXML2_LIB_DIRS)"
	@echo "LIBXML2_LIBS: $(LIBXML2_LIBS)"
endif
ifeq ($(WITH_LIBXSLT),1)
	@echo
	@echo "LIBXSLT_DIR: $(LIBXSLT_DIR)"
	@echo "LIBXSLT_INCLUDE_DIRS: $(LIBXSLT_INCLUDE_DIRS)"
	@echo "LIBXSLT_LIB_DIRS: $(LIBXSLT_LIB_DIRS)"
	@echo "LIBXSLT_LIBS: $(LIBXSLT_LIBS)"
endif
ifeq ($(WITH_LIBHPDF),1)
ifeq ($(WITH_SYSTEM_LIBHPDF),1)
	@echo
	@echo "LIBHPDF_DIR: $(LIBHPDF_DIR)"
	@echo "LIBHPDF_INCLUDE_DIRS: $(LIBHPDF_INCLUDE_DIRS)"
	@echo "LIBHPDF_LIB_DIRS: $(LIBHPDF_LIB_DIRS)"
	@echo "LIBHPDF_LIBS: $(LIBHPDF_LIBS)"
endif
ifeq ($(WITH_LOCAL_LIBHPDF),1)
	@echo
	@echo "using local libhpdf.."
endif
	@echo
	@echo "LIBPNG_DIR: $(LIBPNG_DIR)"
	@echo "LIBPNG_INCLUDE_DIRS: $(LIBPNG_INCLUDE_DIRS)"
	@echo "LIBPNG_LIB_DIRS: $(LIBPNG_LIB_DIRS)"
	@echo "LIBPNG_LIBS: $(LIBPNG_LIBS)"
	@echo
	@echo "LIBZ_DIR: $(LIBZ_DIR)"
	@echo "LIBZ_INCLUDE_DIRS: $(LIBZ_INCLUDE_DIRS)"
	@echo "LIBZ_LIB_DIRS: $(LIBZ_LIB_DIRS)"
	@echo "LIBZ_LIBS: $(LIBZ_LIBS)"
endif
ifeq ($(WITH_LIBGMP),1)
	@echo
	@echo "LIBGMP_DIR: $(LIBGMP_DIR)"
	@echo "LIBGMP_INCLUDE_DIRS: $(LIBGMP_INCLUDE_DIRS)"
	@echo "LIBGMP_LIB_DIRS: $(LIBGMP_LIB_DIRS)"
	@echo "LIBGMP_LIBS: $(LIBGMP_LIBS)"
endif
ifeq ($(WITH_LIBHPDF),1)
ifeq ($(WITH_SYSTEM_LIBHPDF),1)
	@echo
	@echo "LIBHPDF_DIR: $(LIBHPDF_DIR)"
	@echo "LIBHPDF_INCLUDE_DIRS: $(LIBHPDF_INCLUDE_DIRS)"
	@echo "LIBHPDF_LIB_DIRS: $(LIBHPDF_LIB_DIRS)"
	@echo "LIBHPDF_LIBS: $(LIBHPDF_LIBS)"
endif
ifeq ($(WITH_LOCAL_LIBHPDF),1)
	@echo
	@echo "using local libhpdf.."
endif
ifeq ($(WITH_GMP),1)
	@echo
	@echo "LIBGMP_DIR: $(LIBGMP_DIR)"
	@echo "LIBGMP_INCLUDE_DIRS: $(LIBGMP_INCLUDE_DIRS)"
	@echo "LIBGMP_LIB_DIRS: $(LIBGMP_LIB_DIRS)"
	@echo "LIBGMP_LIBS: $(LIBGMP_LIBS)"
endif
endif
	@echo
	@echo "Additional build options:"
	@echo
ifeq ($(ENABLE_NLS),0)
	@echo "NLS support is disabled"
else
	@echo "NLS support is enabled"
endif
ifeq ($(RUN_TESTS),0)
	@echo "Will not execute any tests"
endif
	@echo

.PHONY: init-po
init-po:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d init-po || exit 1); done)

.PHONY: merge-po
merge-po:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d merge-po || exit 1); done)

.PHONY: check-po
check-po:
	@test -z "$(SUBDIRS)" || ( set -e; for d in $(SUBDIRS)""; do \
	  (set -e; $(MAKE) -C $$d check-po || exit 1); done)

-include $(TOPDIR)/makefiles/gmake/dist.mk
