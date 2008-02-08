CROSSDIR = /usr/local/cross-gcc33/i386-mingw32msvc/
CROSSCC = $(CROSSDIR)/bin/gcc
CROSSAR = $(CROSSDIR)/bin/ar
CROSSRANLIB = $(CROSSDIR)/bin/ranlib
OBJDIR = obj/
CC ?= gcc
CXX ?= g++
AR = ar
RANLIB = ranlib

CFLAGS = $(GLOBAL_CFLAGS)
LIBS = $(COMMONLIBS) `gtk-config --libs`

PROJECTNAME = libtt

C_SRCS = action.c alias.c ansi.c antisub.c files.c glob.c help.c \
	highlight.c ivars.c llist.c log.c main.c misc.c net.c parse.c \
	path.c rltab.c function.c walk.c mccpDecompress.c \
	session.c substitute.c text.c ticks.c utils.c variables.c 
    

DEP_SRCS = $(CXX_SRCS) $(C_SRCS)

OBJS = $(patsubst %.c, $(OBJDIR)%.o, $(C_SRCS))

WINOBJS = $(patsubst %.c, $(OBJDIR)%.w, $(C_SRCS))

CROSSCFLAGS = -mwindows -mno-cygwin -mms-bitfields $(GLOBAL_CFLAGS)

# -fnative-struct corrispondente di -mms-bifields nel vecchio mingw32 -s

all: depend $(PROJECTNAME).a

win32: depend $(PROJECTNAME)w32.a

$(OBJDIR)%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR)%.o: %.cpp
	$(CXX) $(CFLAGS) -o $@ -c $<

$(OBJDIR)%.w: %.cpp
	$(CROSSCXX) $(CROSSCFLAGS) -o $@ -c $<

$(OBJDIR)%.w: %.c
	$(CROSSCC) $(CROSSCFLAGS) -o $@ -c $<

$(PROJECTNAME).a: $(OBJS)
	$(AR) cru $@ $(OBJS)
	$(RANLIB) $@
	
$(PROJECTNAME)w32.a: $(WINOBJS)
	$(CROSSAR) cru $@ $(WINOBJS)
	$(CROSSRANLIB) $@

clean: 
	rm -f $(WINOBJS) $(OBJS) $(PROJECTNAME)w32.a $(PROJECTNAME).a depend

depend:	$(C_SRCS) *.h
	for i in $(C_SRCS); do \
		$(CC) $(CFLAGS) -c -M $$i; \
	done >  deptemp
	sed -f ../mkdep.sed deptemp >depend
	rm deptemp
    
-include depend

