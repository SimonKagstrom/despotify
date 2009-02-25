#
# $Id$
# 
#
CFLAGS = -Wall -ggdb -DGUI
#CFLAGS = -Wall -ggdb -DGUI -DDEBUG
LDFLAGS = -lcrypto -lz -lexpat -lvorbisfile 

CORE_OBJS = aes.o auth.o buffer.o channel.o commands.o dns.o event.o handlers.o keyexchange.o packet.o playlist.o puzzle.o session.o shn.o util.o xml.o
DESPOTIFY_OBJS = ADMclubbing.o audio.o despotify.o ui-core.o ui-help.o ui-player.o ui-playlist.o ui-search.o sndqueue.o
GATEWAY_OBJS = gw-core.o gw-browse.o gw-handlers.o gw-playlist.o gw-search.o gw-stream.o gw-http.o base64.o

# Mac OS X specifics
ifeq ($(shell uname -s),Darwin)
	LDFLAGS += -lresolv -framework CoreAudio
	DESPOTIFY_OBJS += coreaudio.o
endif

# Linux specifics
ifeq ($(shell uname -s),Linux)
	LDFLAGS += -lpulse -lpulse-simple
	DESPOTIFY_OBJS += pulseaudio.o
endif


# FreeBSD specifics
ifeq ($(shell uname -s),FreeBSD)
	CFLAGS += -I/usr/local/include
	LDFLAGS += -L/usr/local/include -lpulse -lpulse-simple
	DESPOTIFY_OBJS += pulseaudio.o
endif


all: core despotify gateway


clean:
	rm -f $(CORE_OBJS) $(DESPOTIFY_OBJS) $(GATEWAY_OBJS) despotify gateway

core: $(CORE_OBJS)

despotify: core $(DESPOTIFY_OBJS)
	gcc -o $@ $(LDFLAGS) -lncurses $(CORE_OBJS) $(DESPOTIFY_OBJS)

gateway: core $(GATEWAY_OBJS)
	gcc -o $@ $(LDFLAGS) $(CORE_OBJS) $(GATEWAY_OBJS)

privacy:
	echo *.c *h|xargs -n1 -t indent -bacc -bc -nce  -br -di1 -eei -nip -npsl -sob -nbad -nbc -i8
