#
# $Id$
# 
#

export

CFLAGS = -Wall -Wextra -ggdb
LDFLAGS = -lcrypto -lz -lexpat -lvorbisfile
LD = $(CC)

LINUX_BACKEND = gstreamer

include Makefile.local.mk

ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG
endif

# Mac OS X specifics
ifeq ($(shell uname -s),Darwin)
	LDFLAGS += -lresolv -framework CoreAudio
endif

# Linux specifics
ifeq ($(shell uname -s),Linux)
    LDFLAGS += -lresolv
    ifeq ($(LINUX_BACKEND),gstreamer)
	CFLAGS += $(shell pkg-config --cflags gstreamer-base-0.10)
	LDFLAGS += $(shell pkg-config --libs-only-l --libs-only-L gstreamer-base-0.10)
    endif

    ifeq ($(LINUX_BACKEND),libao)
	LDFLAGS += -lao
    endif

    ifeq ($(LINUX_BACKEND),pulseaudio)
	LDFLAGS += -lpulse -lpulse-simple
    endif
endif

# FreeBSD specifics
ifeq ($(shell uname -s),FreeBSD)
	CFLAGS += -I/usr/local/include
	LDFLAGS += -L/usr/local/include -lpulse -lpulse-simple
endif

CLIENTS = clients/despotify clients/gateway
SUBDIRS = lib $(CLIENTS)

all: $(SUBDIRS)

clean: 
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

.PHONY: all clean $(SUBDIRS) install uninstall

$(SUBDIRS):
	$(MAKE) -C $@

clients: $(CLIENTS)

install:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir install; \
	done

uninstall:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir uninstall; \
	done

Makefile.local.mk:
	@echo " **** No Makefile.local.mk found, copying dist."
	cp Makefile.local.mk.dist Makefile.local.mk
