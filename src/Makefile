#
# $Id$
# 
#
CFLAGS = -Wall -Wextra
LDFLAGS = -lcrypto -lz -lexpat -lvorbisfile 

CORE_OBJS = aes.o auth.o buffer.o channel.o commands.o dns.o event.o handlers.o keyexchange.o packet.o playlist.o puzzle.o session.o shn.o util.o xml.o
DESPOTIFY_OBJS = ADMclubbing.o audio.o despotify.o ui-core.o ui-help.o ui-player.o ui-playlist.o ui-search.o sndqueue.o
GATEWAY_OBJS = gw-core.o gw-browse.o gw-handlers.o gw-playlist.o gw-search.o gw-stream.o gw-http.o base64.o

LINUX_BACKEND = gstreamer

include Makefile.local.mk

ifdef DEBUG
    CFLAGS += -DGUI -ggdb
endif

ifndef NOGUI
    CFLAGS += -DGUI
endif

# Mac OS X specifics
ifeq ($(shell uname -s),Darwin)
	LDFLAGS += -lresolv -framework CoreAudio
	DESPOTIFY_OBJS += coreaudio.o
endif

# Linux specifics
ifeq ($(shell uname -s),Linux)
    LDFLAGS += -lresolv
    ifeq ($(LINUX_BACKEND),gstreamer)
	CFLAGS += `pkg-config --cflags gstreamer-base-0.10`
	LDFLAGS += `pkg-config --libs gstreamer-base-0.10`
	DESPOTIFY_OBJS += gstreamer.o gstapp/libgstapp.a
	endif

	ifeq ($(LINUX_BACKEND),libao)
	LDFLAGS += -lao
	DESPOTIFY_OBJS += libao.o
	endif

	ifeq ($(LINUX_BACKEND),pulseaudio)
	LDFLAGS += -lpulse -lpulse-simple
	DESPOTIFY_OBJS += pulseaudio.o
    endif
endif


# FreeBSD specifics
ifeq ($(shell uname -s),FreeBSD)
	CFLAGS += -I/usr/local/include
	LDFLAGS += -L/usr/local/include -lpulse -lpulse-simple
	DESPOTIFY_OBJS += pulseaudio.o
endif


all: core despotify gateway

D=$(CORE_OBJS:.o=.d) $(DESPOTIFY_OBJS:.o=.d) $(GATEWAY_OBJS:.o=.d)
include $(D)
deps: $(D)
%.d: %.c
	$(SHELL) -ec '$(CXX) -MM $(CFLAGS) $(DEFINES) $< \
		| sed '\''s/$*\\.o[ :]*/& $@/g'\'' > $@'

gstapp/libgstapp.a:
	make -C gstapp

clean:
	make -C gstapp clean
	rm -f $(CORE_OBJS) $(DESPOTIFY_OBJS) $(GATEWAY_OBJS) $(D) despotify gateway

core: $(CORE_OBJS)

despotify: core $(DESPOTIFY_OBJS)
	gcc -o $@ $(LDFLAGS) -lncurses $(CORE_OBJS) $(DESPOTIFY_OBJS)

gateway: core $(GATEWAY_OBJS)
	gcc -o $@ $(LDFLAGS) $(CORE_OBJS) $(GATEWAY_OBJS)

privacy:
	echo *.c *h|xargs -n1 -t indent -bacc -bc -nce  -br -di1 -eei -nip -npsl -sob -nbad -nbc -i8

Makefile.local.mk:
	@echo " **** No Makefile.local.mk found, copying dist."
	cp Makefile.local.mk.dist Makefile.local.mk
