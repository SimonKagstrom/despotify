#
# $Id$
# 

LIB_OBJS = aes.lo auth.lo buf.lo channel.lo commands.lo dns.lo ezxml.lo handlers.lo keyexchange.lo packet.lo puzzle.lo session.lo shn.lo sndqueue.lo util.lo network.lo despotify.lo sha1.lo hmac.lo xml.lo 

LDFLAGS += -rpath /usr/lib
LDCONFIG = ldconfig

.PHONY: all clean install uninstall

all: libdespotify.la

# Mac OS X specifics
ifeq ($(shell uname -s),Darwin)
    LDFLAGS += -lresolv
    LDCONFIG = true
endif

# Windows specifics
ifeq ($(firstword $(subst _, ,$(shell uname -s))), MINGW32)
    LDFLAGS += -lwsock32 -lDnsapi
endif

# Linux specifics
ifeq ($(shell uname -s),Linux)
    LDFLAGS += -lresolv
endif

libdespotify.la: $(LIB_OBJS)
	@echo LD $@
	$(SILENT)$(LT) --mode=link $(CC) -o libdespotify.la $(LDFLAGS) $(LIB_OBJS)

%.lo: %.c
	@echo CC $<
	$(SILENT)$(LT) --mode=compile $(CC) $(CFLAGS) -o $@ -c $<

ifeq (,$(filter clean, $(MAKECMDGOALS))) # don't make deps for "make clean"
CFILES = $(LIB_OBJS:.lo=.c)

Makefile.dep: $(CFILES)
	@echo Generating dependencies
	$(SILENT)$(CC) $(CFLAGS) -MM $(CFILES) | sed 's/^\([^ ]\+\).o:/\1.lo:/' > $@

-include Makefile.dep
endif

clean:
	$(LT) --mode=clean rm -f $(LIB_OBJS) Makefile.dep

install: libdespotify.la
	install -d $(INSTALL_PREFIX)/lib/pkgconfig
	 
	$(LT) --mode=install install libdespotify.la $(INSTALL_PREFIX)/lib/libdespotify.la
	$(LDCONFIG) -n $(INSTALL_PREFIX)/lib
	install -d $(INSTALL_PREFIX)/include/ 
	install despotify.h $(INSTALL_PREFIX)/include/
	install -d $(INSTALL_PREFIX)/lib/pkgconfig/
	install despotify.pc $(INSTALL_PREFIX)/lib/pkgconfig/despotify.pc

uninstall:
	$(LT) --mode=uninstall rm -f $(INSTALL_PREFIX)/lib/libdespotify.la
	
	rm -fr $(INSTALL_PREFIX)/include/despotify/
	rm -f $(INSTALL_PREFIX)/lib/pkgconfig/despotify.pc
