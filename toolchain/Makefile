BINUTILS_VER=2.19.1
BINUTILS_URL=http://mirrors.kernel.org/gnu/binutils/binutils-$(BINUTILS_VER).tar.bz2
BINUTILS_TAR=binutils-$(BINUTILS_VER).tar.bz2

NEWLIB_VER=1.17.0
NEWLIB_URL=ftp://sources.redhat.com/pub/newlib/newlib-$(NEWLIB_VER).tar.gz
NEWLIB_TAR=newlib-$(NEWLIB_VER).tar.gz

GCC_VER=4.3.3
GCC_URL=http://mirrors.kernel.org/gnu/gcc/gcc-$(GCC_VER)/gcc-core-$(GCC_VER).tar.bz2
GCC_TAR=gcc-core-$(GCC_VER).tar.bz2

BASEDIR=$(shell pwd)

all: gcc

src/$(BINUTILS_TAR):
	wget -P src $(BINUTILS_URL)

src/$(NEWLIB_TAR):
	wget -P src $(NEWLIB_URL)

src/$(GCC_TAR):
	wget -P src $(GCC_URL)

src/binutils-$(BINUTILS_VER): src/$(BINUTILS_TAR)
	tar -C src -xjf $<

src/newlib-$(NEWLIB_VER): src/$(NEWLIB_TAR)
	tar -C src -xzf $<

src/gcc-$(GCC_VER): src/$(GCC_TAR) src/newlib-$(NEWLIB_VER)
	tar -C src -xjf $<
	ln -s $(BASEDIR)/src/newlib-$(NEWLIB_VER)/{newlib,libgloss} $@

binutils: src/binutils-$(BINUTILS_VER)
	mkdir -p build/binutils
	cd build/binutils; \
	$(BASEDIR)/$</configure --target=sh-elf --prefix=$(BASEDIR)/inst; \
	$(MAKE); \
	$(MAKE) install

gcc: src/gcc-$(GCC_VER) binutils
	mkdir -p build/gcc
	cd build/gcc; \
	$(BASEDIR)/$</configure --target=sh-elf --prefix=$(BASEDIR)/inst -enable-languages=c --without-pkgversion --with-newlib; \
	$(MAKE); \
	$(MAKE) install

clean:
	rm -rf build inst

distclean: clean
	rm -rf src