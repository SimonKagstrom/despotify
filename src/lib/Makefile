#
# $Id$
# 

CFLAGS += -fPIC
LDFLAGS += -Wl,-soname,libdespotify.so.1
LIB_OBJS += aes.o audio.o auth.o buffer.o channel.o commands.o dns.o handlers.o keyexchange.o packet.o playlist.o puzzle.o session.o shn.o sndqueue.o util.o xml.o

# Mac OS X specifics
ifeq ($(shell uname -s),Darwin)
	LIB_OBJS += coreaudio.o
endif

# Linux specifics
ifeq ($(shell uname -s),Linux)
    ifeq ($(LINUX_BACKEND),gstreamer)
	LIB_OBJS += gstreamer.o gstapp/libgstapp.a
    endif

    ifeq ($(LINUX_BACKEND),libao)
	LIB_OBJS += libao.o
    endif

    ifeq ($(LINUX_BACKEND),pulseaudio)
	LIB_OBJS += pulseaudio.o
    endif
endif

# FreeBSD specifics
ifeq ($(shell uname -s),FreeBSD)
	LIB_OBJS += pulseaudio.o
endif

all: libdespotify.so

libdespotify.so: $(LIB_OBJS)
	$(LD) -shared -o libdespotify.so $(LDFLAGS) $^

ifeq (,$(filter clean, $(MAKECMDGOALS))) # don't make deps for "make clean"
CFILES = $(filter-out %.a,$(LIB_OBJS:.o=.c))

Makefile.dep:
	$(CC) $(CFLAGS) -MM $(CFILES) > $@

-include Makefile.dep
endif

.PHONY: all gstapp/libgstapp.a clean

gstapp/libgstapp.a:
	$(MAKE) -C gstapp

clean:
	$(MAKE) -C gstapp clean
	rm -f $(LIB_OBJS) Makefile.dep

install: libdespotify.so
	# install despotify.h /usr/include
	install libdespotify.so /usr/lib/libdespotify.so.1.0.0
	ldconfig -n /usr/lib
	ln -sf libdespotify.so.1 /usr/lib/libdespotify.so

uninstall:
	# rm -f /usr/include/despotify.h 
	rm -f /usr/lib/libdespotify.so*
