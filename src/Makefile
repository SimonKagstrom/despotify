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
    LT = glibtool --tag=CC
else
    LT = libtool --tag=CC
endif


CLIENTS = clients/despotify clients/gateway
SUBDIRS = lib $(CLIENTS)

all: $(SUBDIRS)

clean: 
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir -f local.mk clean || exit $?; \
	done

.PHONY: all clean $(SUBDIRS) install uninstall

$(SUBDIRS):
	$(MAKE) -C $@ -f local.mk || exit $?

clients: $(CLIENTS)

install:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir -f local.mk install || exit $?; \
	done

uninstall:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir -f local.mk uninstall || exit $?; \
	done

Makefile.local.mk:
	@echo " **** No Makefile.local.mk found, copying dist."
	cp Makefile.local.mk.dist Makefile.local.mk
