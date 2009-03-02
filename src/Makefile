#
# $Id$
# 
#

export

CFLAGS = -Wall -Wextra -ggdb -Wno-pointer-sign
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

.PHONY: all clean $(SUBDIRS) install uninstall

all: $(SUBDIRS)

clean: 
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir -f local.mk clean || exit $$?; \
	done

clients: $(CLIENTS)
$(CLIENTS): lib
$(SUBDIRS):
	$(MAKE) -C $@ -f local.mk

install: $(SUBDIRS)
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir -f local.mk install || exit $$?; \
	done

uninstall:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir -f local.mk uninstall || exit $$?; \
	done

Makefile.local.mk:
	@echo " **** No Makefile.local.mk found, copying dist."
	cp Makefile.local.mk.dist Makefile.local.mk
