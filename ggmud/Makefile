CROSSDIR = /usr/local/cross-gcc33/i386-mingw32msvc/
CROSSCC = $(CROSSDIR)/bin/gcc
GLOBAL_CFLAGS = -g
OBJDIR = obj/
CC = gcc
CXX = g++
INCDIR = -Itt
COMMONLIBS = -Ltt -ltt
CFLAGS = $(INCDIR) `gtk-config --cflags` $(GLOBAL_CFLAGS)
LIBS = $(COMMONLIBS) `gtk-config --libs`
VERSION := $(shell grep VERSION config.h | cut -d\" -f2 )

export GLOBAL_CFLAGS

PROJECTNAME = ggmud

C_SRCS = alias.c ansi.c font.c fileopen.c help.c history.c log.c logviewer.c \
         macro.c net.c preferences.c ggmud.c triggers.c window.c wizard.c \
         variables.c highlights.c gags.c timers.c

DEP_SRCS = $(CXX_SRCS) $(C_SRCS)

OBJS = $(patsubst %.c, $(OBJDIR)%.o, $(C_SRCS))

WINOBJS = $(OBJDIR)icon.w $(patsubst %.c, $(OBJDIR)%.w, $(C_SRCS))

CROSSCFLAGS = $(INCDIR) -I$(CROSSDIR)/include/glib-2.0 -mwindows \
			-I$(CROSSDIR)/include/gtk -mno-cygwin -mms-bitfields $(GLOBAL_CFLAGS)

# -fnative-struct corrispondente di -mms-bifields nel vecchio mingw32 -s

CROSSLIBS = -s -L../lib_w32 -Ltt -lttw32 -lgtk.dll -lgdk.dll -lgmodule-2.0.dll -lglib-2.0.dll -lws2_32

all: depend $(PROJECTNAME) 

win32: depend $(PROJECTNAME).exe

$(OBJDIR)%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR)%.o: %.cpp
	$(CXX) $(CFLAGS) -o $@ -c $<

$(OBJDIR)%.w: %.cpp
	$(CROSSCXX) $(CROSSCFLAGS) -o $@ -c $<

$(OBJDIR)%.w: %.c
	$(CROSSCC) $(CROSSCFLAGS) -o $@ -c $<

$(OBJDIR)icon.w: icon.rc icon.ico
	$(CROSSDIR)/../bin/i386-mingw32msvc-windres $< $@

tt/libtt.a: tt/*.c
	cd tt && make && cd ..
    
tt/libttw32.a: tt/*.c
	cd tt && make win32 && cd ..

$(PROJECTNAME): $(OBJS) tt/libtt.a
	$(CC) $(CFLAGS) $(OBJS) -o $(PROJECTNAME) $(LIBS)

$(PROJECTNAME).exe: $(WINOBJS) tt/libttw32.a
	$(CROSSCC) $(CROSSCFLAGS) $(WINOBJS) -o $(PROJECTNAME).exe $(CROSSLIBS)

clean: 
	cd tt && make clean && cd ..
	rm -f $(WINOBJS) $(OBJS) $(PROJECTNAME) $(PROJECTNAME).exe depend 

dist: dist-osx dist-linux dist-win32
    
dist-osx: ggmud
	tar czf ../ggmud-osx-$(VERSION).tgz ggmud gg_help.txt README README.OSX

dist-linux: ggmud
	tar czf ../ggmud-linux-$(VERSION).tgz ggmud gg_help.txt README

dist-win32: ggmud.exe
	zip -9 ../ggmud-win32-$(VERSION).zip ggmud.exe gg_help.txt README README.W32

depend:	$(C_SRCS) *.h
	for i in $(C_SRCS); do \
		$(CC) $(CFLAGS) -c -M $$i; \
	done >  deptemp
	sed -f mkdep.sed deptemp >depend
	rm deptemp
        
-include depend
