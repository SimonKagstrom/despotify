#
# $Id$
# 

unexport LDFLAGS CFLAGS

LIBDIR = ../../lib

CFLAGS += -I$(LIBDIR)
LDFLAGS += $(LIBDIR)/libdespotify.la

GATEWAY_OBJS = gw-core.o gw-browse.o gw-handlers.o gw-image.o gw-playlist.o gw-search.o gw-stream.o gw-http.o base64.o

all: gateway

.PHONY: all clean lib install uninstall

ifeq (,$(filter clean, $(MAKECMDGOALS))) # don't make deps for "make clean"
CFILES = $(filter-out %.a,$(GATEWAY_OBJS:.o=.c))

Makefile.dep:
	$(CC) $(CFLAGS) -MM $(CFILES) > $@

-include Makefile.dep
endif

clean:
	$(LT) --mode=clean rm -f gateway
	rm -f $(GATEWAY_OBJS) Makefile.dep

gateway: $(GATEWAY_OBJS) lib
	$(LT) --mode=link $(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(GATEWAY_OBJS)

lib:
	$(MAKE) -C $(LIBDIR)

install: gateway 
	@echo "Copying gateway binary to /usr/bin/gateway"
	$(LT) --mode=install install gateway /usr/bin/gateway

uninstall:
	@echo "Removing gateway..."
	rm -f /usr/bin/gateway
