#
# $Id$
# 

CFLAGS += -Igstapp/
DEP_CC := $(CC)
CC := $(LT) --mode=compile $(CC)

LDFLAGS += -rpath /usr/lib

LIB_OBJS += aes.o audio.o auth.o buffer.o channel.o commands.o dns.o esbuf.o handlers.o keyexchange.o packet.o playlist.o puzzle.o session.o shn.o sndqueue.o util.o xml.o

.PHONY: all clean

all: libdespotify.la

# Mac OS X specifics
ifeq ($(shell uname -s),Darwin)
    LIB_OBJS += coreaudio.o
    LDFLAGS += -lresolv -framework CoreAudio
endif

# Linux specifics
ifeq ($(shell uname -s),Linux)
    LDFLAGS += -lresolv
    ifeq ($(LINUX_BACKEND),gstreamer)
        CFLAGS += $(shell pkg-config --cflags gstreamer-base-0.10)
        LDFLAGS += $(shell pkg-config --libs-only-l --libs-only-L gstreamer-base-0.10)

        LIB_OBJS += gstreamer.o
        LIB_OBJS += gstapp/gstappsrc.o gstapp/gstappbuffer.o gstapp/gstapp-marshal.o

gstapp/gstapp-marshal.h: gstapp/gstapp-marshal.list
	glib-genmarshal --header --prefix=gst_app_marshal gstapp/gstapp-marshal.list > gstapp/gstapp-marshal.h.tmp
	mv gstapp/gstapp-marshal.h.tmp gstapp/gstapp-marshal.h

gstapp/gstapp-marshal.c: gstapp/gstapp-marshal.list gstapp/gstapp-marshal.h
	echo "#include \"gstapp-marshal.h\"" >> gstapp/gstapp-marshal.c.tmp
	glib-genmarshal --body --prefix=gst_app_marshal gstapp/gstapp-marshal.list >> gstapp/gstapp-marshal.c.tmp
	mv gstapp/gstapp-marshal.c.tmp gstapp/gstapp-marshal.c

    endif

    ifeq ($(LINUX_BACKEND),libao)
        LIB_OBJS += libao.o
        LDFLAGS += -lao
    endif

    ifeq ($(LINUX_BACKEND),pulseaudio)
        LIB_OBJS += pulseaudio.o
        LDFLAGS += -lpulse -lpulse-simple
    endif
endif

# FreeBSD specifics
ifeq ($(shell uname -s),FreeBSD)
    LIB_OBJS += pulseaudio.o
    CFLAGS += -I/usr/local/include
    LDFLAGS += -L/usr/local/include -lpulse -lpulse-simple
endif

libdespotify.la: $(LIB_OBJS)
	$(LT) --mode=link $(LD) -o libdespotify.la $(LDFLAGS) $(LIB_OBJS:.o=.lo)

ifeq (,$(filter clean, $(MAKECMDGOALS))) # don't make deps for "make clean"
CFILES = $(LIB_OBJS:.o=.c)

Makefile.dep: $(CFILES)
	$(DEP_CC) $(CFLAGS) -MM $(CFILES) > $@

-include Makefile.dep
endif

clean:
	$(LT) --mode=clean rm -f $(notdir $(LIB_OBJS:.o=.lo)) Makefile.dep

install: libdespotify.la
	# install despotify.h /usr/include
	$(LT) --mode=install install libdespotify.la /usr/lib/libdespotify.la
	ldconfig -n /usr/lib

uninstall:
	# rm -f /usr/include/despotify.h 
	$(LT) --mode=uninstall rm -f /usr/lib/libdespotify.la
