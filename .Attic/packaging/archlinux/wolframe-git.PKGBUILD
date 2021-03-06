# Contributor: Andreas Baumann <abaumann at yahoo dot com>
# Maintainer: Andreas Baumann <abaumann at yahoo dot com>
pkgname=wolframe-git
pkgver=release_0.0.2.r319.g0c7abd2
pkgrel=1
pkgdesc="A flexible client-server ecosystem for business applications."
license=('GPL3')
arch=('i686' 'x86_64')
url="http://wolframe.net/"
depends=('boost>=1.48' 'boost-libs>=1.48' 'openssl' 'pam' 'libsasl' 
         'sqlite3' 'postgresql-libs' 'libxml2' 'libxslt'
         'python')
makedepends=('git' 'docbook-xsl' 'doxygen' 'fop' 'graphviz' 'dia')
checkdepends=('expect' 'inetutils' 'diffutils')
conflicts=('wolframe')
provides=('wolframe')
backup=('etc/wolframe/wolframe.conf')
install='wolframe.install' 
source=(
  'git://github.com/Wolframe/Wolframe.git'
  'wolframed.service'
)
md5sums=('SKIP'
         'cc146a28908aad55f156860340934095')

pkgver() {
   cd ${srcdir}/Wolframe
   git describe --long --tags | sed -r 's/([^-]*-g)/r\1/;s/-/./g'
}

package() {
  cd ${srcdir}/Wolframe

  msg "Installing.."
  make \
    WITH_SSL=1 WITH_EXPECT=1 WITH_PAM=1 WITH_SASL=1 \
    WITH_SYSTEM_SQLITE3=1 WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 \
    WITH_LIBXSLT=1 WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 \
    WITH_LOCAL_FREEIMAGE=1 WITH_PYTHON=1 WITH_CJSON=1 \
    WITH_TEXTWOLF=1 RELEASE=1 \
    DESTDIR=${pkgdir} prefix=/usr sbindir=/usr/bin \
    sysconfdir=/etc \
    install

  install -D -m644 ${srcdir}/wolframed.service \
    ${pkgdir}/usr/lib/systemd/system/wolframed.service
}

check() {
  cd ${srcdir}/Wolframe

  msg "Testing..."
  make test \
    WITH_SSL=1 WITH_EXPECT=1 WITH_PAM=1 WITH_SASL=1 \
    WITH_SYSTEM_SQLITE3=1 WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 \
    WITH_LIBXSLT=1 WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 \
    WITH_LOCAL_FREEIMAGE=1 WITH_PYTHON=1 WITH_CJSON=1 \
    WITH_TEXTWOLF=1 RELEASE=1 \
    DEFAULT_MODULE_LOAD_DIR=/usr/lib/wolframe/modules \
    RELEASE=1 \
    LDFLAGS=-Wl,-rpath=/usr/lib/wolframe,--enable-new-dtags
}

build() {
  cd ${srcdir}/Wolframe

  msg "Patching build number in software.."
  GIT_COMMIT_COUNT=`git describe --long --tags | cut -f 2 -d -`
  sed -i "s/^#define WOLFRAME_BUILD.*/#define WOLFRAME_BUILD $GIT_COMMIT_COUNT/g" ${srcdir}/Wolframe/include/wolframe.hpp
  
  msg "Generating dependencies..."
  make depend \
    WITH_SSL=1 WITH_EXPECT=1 WITH_PAM=1 WITH_SASL=1 \
    WITH_SYSTEM_SQLITE3=1 WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 \
    WITH_LIBXSLT=1 WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 \
    WITH_LOCAL_FREEIMAGE=1 WITH_PYTHON=1 WITH_CJSON=1 \
    WITH_TEXTWOLF=1 RELEASE=1 \
    DEFAULT_MODULE_LOAD_DIR=/usr/lib/wolframe/modules \
    RELEASE=1 \
    LDFLAGS=-Wl,-rpath=/usr/lib/wolframe,--enable-new-dtags

  msg "Building..."
  make \
    WITH_SSL=1 WITH_EXPECT=1 WITH_PAM=1 WITH_SASL=1 \
    WITH_SYSTEM_SQLITE3=1 WITH_PGSQL=1 WITH_LUA=1 WITH_LIBXML2=1 \
    WITH_LIBXSLT=1 WITH_LOCAL_LIBHPDF=1 WITH_ICU=1 \
    WITH_LOCAL_FREEIMAGE=1 WITH_PYTHON=1 WITH_CJSON=1 \
    WITH_TEXTWOLF=1 RELEASE=1 \
    DEFAULT_MODULE_LOAD_DIR=/usr/lib/wolframe/modules \
    RELEASE=1 \
    LDFLAGS=-Wl,-rpath=/usr/lib/wolframe,--enable-new-dtags

#  msg "Building documentation.."
#  cd docs
#  make doc
  cd ..
}
