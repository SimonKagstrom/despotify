#
# $Id: local.mk 489 2010-01-20 16:09:33Z fxbhh $
# 

LIB_OBJS = aes.o auth.o buf.o cache.o channel.o commands.o dns.o ezxml.o handlers.o keyexchange.o packet.o puzzle.o session.o shn.o sndqueue.o util.o network.o despotify.o sha1.o hmac.o xml.o 

LDFLAGS += -rpath /usr/lib
LDCONFIG = ldconfig

.PHONY: all clean install uninstall

all: libdespotify.a

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
    LDFLAGS += -lresolv -lpthread
endif

libdespotify.a: $(LIB_OBJS)
	@echo LD $@
	$(SILENT)ar crs $@ $(LIB_OBJS)

%.o: %.c
	@echo CC $<
	$(SILENT) $(CC) $(CFLAGS) -o $@ -c $<

ifeq (,$(filter clean, $(MAKECMDGOALS))) # don't make deps for "make clean"
CFILES = $(LIB_OBJS:.o=.c)

Makefile.dep: $(CFILES)
	@echo Generating dependencies
	$(SILENT)$(CC) $(CFLAGS) -MM $(CFILES) | sed 's/^\([^ ]\+\).o:/\1.o:/' > $@

-include Makefile.dep
endif

clean:
	$(LT) --mode=clean rm -f $(LIB_OBJS) Makefile.dep

install: libdespotify.a
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
