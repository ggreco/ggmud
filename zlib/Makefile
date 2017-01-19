# Makefile for zlib
# Copyright (C) 1995-2005 Jean-loup Gailly.
# For conditions of distribution and use, see copyright notice in zlib.h

# To compile and test, type:
#    ./configure; make test
# The call of configure is optional if you don't have special requirements
# If you wish to build zlib as a shared library, use: ./configure -s

# To use the asm code, type:
#    cp contrib/asm?86/match.S ./match.S
#    make LOC=-DASMV OBJA=match$(OBJEXT)

# To install /usr/local/lib/libz.* and /usr/local/include/zlib.h, type:
#    make install
# To install in $HOME instead of /usr/local, use:
#    make install prefix=$HOME

CC?=cc
OBJEXT?=.o
CFLAGS=$(DEBUGFLAG) -Wall
#CFLAGS=-O -DMAX_WBITS=14 -DMAX_MEM_LEVEL=7
#CFLAGS=-g -DDEBUG
#CFLAGS=-O3 -Wall -Wwrite-strings -Wpointer-arith -Wconversion \
#           -Wstrict-prototypes -Wmissing-prototypes

LDFLAGS=libzlib.a
LDSHARED=$(CC)
CPP=$(CC) -E

LIBS=libzlib.a
SHAREDLIB=libz.so
SHAREDLIBV=libz.so.1.2.3
SHAREDLIBM=libz.so.1

AR?=ar
RANLIB?=ranlib
TAR=tar
SHELL=/bin/sh
EXE=

prefix = /usr/local
exec_prefix = ${prefix}
libdir = ${exec_prefix}/lib
includedir = ${prefix}/include
mandir = ${prefix}/share/man
man3dir = ${mandir}/man3

OBJS = adler32$(OBJEXT) compress$(OBJEXT) crc32$(OBJEXT) \
	   gzio$(OBJEXT) uncompr$(OBJEXT) deflate$(OBJEXT) trees$(OBJEXT) \
       zutil$(OBJEXT) inflate$(OBJEXT) infback$(OBJEXT) \
	   inftrees$(OBJEXT) inffast$(OBJEXT)

OBJA =
# to use the asm code: make OBJA=match$(OBJEXT)

TEST_OBJS = example$(OBJEXT) minigzip$(OBJEXT)

all: example$(EXE) minigzip$(EXE)

check: test
test: all
	@LD_LIBRARY_PATH=.:$(LD_LIBRARY_PATH) ; export LD_LIBRARY_PATH; \
	echo hello world | ./minigzip | ./minigzip -d || \
	  echo '		*** minigzip test FAILED ***' ; \
	if ./example; then \
	  echo '		*** zlib test OK ***'; \
	else \
	  echo '		*** zlib test FAILED ***'; \
	fi

%.obj: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

libzw32.a: $(OBJS) $(OBJA)
	$(AR) rc $@ $(OBJS) $(OBJA)
	$(RANLIB) $@ 

libzlib.a: $(OBJS) $(OBJA)
	$(AR) rc $@ $(OBJS) $(OBJA)
	$(RANLIB) $@ 

match$(OBJEXT): match.S
	$(CPP) match.S > _match.s
	$(CC) -c _match.s
	mv _match$(OBJEXT) match$(OBJEXT)
	rm -f _match.s

$(SHAREDLIBV): $(OBJS)
	$(LDSHARED) -o $@ $(OBJS)
	rm -f $(SHAREDLIB) $(SHAREDLIBM)
	ln -s $@ $(SHAREDLIB)
	ln -s $@ $(SHAREDLIBM)

example$(EXE): example$(OBJEXT) $(LIBS)
	$(CC) $(CFLAGS) -o $@ example$(OBJEXT) $(LDFLAGS)

minigzip$(EXE): minigzip$(OBJEXT) $(LIBS)
	$(CC) $(CFLAGS) -o $@ minigzip$(OBJEXT) $(LDFLAGS)

install: $(LIBS)
	-@if [ ! -d $(exec_prefix) ]; then mkdir -p $(exec_prefix); fi
	-@if [ ! -d $(includedir)  ]; then mkdir -p $(includedir); fi
	-@if [ ! -d $(libdir)      ]; then mkdir -p $(libdir); fi
	-@if [ ! -d $(man3dir)     ]; then mkdir -p $(man3dir); fi
	cp zlib.h zconf.h $(includedir)
	chmod 644 $(includedir)/zlib.h $(includedir)/zconf.h
	cp $(LIBS) $(libdir)
	cd $(libdir); chmod 755 $(LIBS)
	-@(cd $(libdir); $(RANLIB) libzlib.a || true) >/dev/null 2>&1
	cd $(libdir); if test -f $(SHAREDLIBV); then \
	  rm -f $(SHAREDLIB) $(SHAREDLIBM); \
	  ln -s $(SHAREDLIBV) $(SHAREDLIB); \
	  ln -s $(SHAREDLIBV) $(SHAREDLIBM); \
	  (ldconfig || true)  >/dev/null 2>&1; \
	fi
	cp zlib.3 $(man3dir)
	chmod 644 $(man3dir)/zlib.3
# The ranlib in install is needed on NeXTSTEP which checks file times
# ldconfig is for Linux

uninstall:
	cd $(includedir); \
	cd $(libdir); rm -f libzlib.a; \
	if test -f $(SHAREDLIBV); then \
	  rm -f $(SHAREDLIBV) $(SHAREDLIB) $(SHAREDLIBM); \
	fi
	cd $(man3dir); rm -f zlib.3

mostlyclean: clean
clean:
	rm -f *$(OBJEXT) *.a *~ example$(EXE) minigzip$(EXE) \
	   libz.* foo.gz so_locations \
	   _match.s maketree contrib/infback9/*$(OBJEXT)

maintainer-clean: distclean
distclean: clean
	cp -p Makefile.in Makefile
	cp -p zconf.in.h zconf.h
	rm -f .DS_Store

tags:
	etags *.[ch]

depend:
	makedepend -- $(CFLAGS) -- *.[ch]

# DO NOT DELETE THIS LINE -- make depend depends on it.

adler32$(OBJEXT): zlib.h zconf.h
compress$(OBJEXT): zlib.h zconf.h
crc32$(OBJEXT): crc32.h zlib.h zconf.h
deflate$(OBJEXT): deflate.h zutil.h zlib.h zconf.h
example$(OBJEXT): zlib.h zconf.h
gzio$(OBJEXT): zutil.h zlib.h zconf.h
inffast$(OBJEXT): zutil.h zlib.h zconf.h inftrees.h inflate.h inffast.h
inflate$(OBJEXT): zutil.h zlib.h zconf.h inftrees.h inflate.h inffast.h
infback$(OBJEXT): zutil.h zlib.h zconf.h inftrees.h inflate.h inffast.h
inftrees$(OBJEXT): zutil.h zlib.h zconf.h inftrees.h
minigzip$(OBJEXT): zlib.h zconf.h
trees$(OBJEXT): deflate.h zutil.h zlib.h zconf.h trees.h
uncompr$(OBJEXT): zlib.h zconf.h
zutil$(OBJEXT): zutil.h zlib.h zconf.h
