OBJS = maemify.o

LIBDIR = ../../lib
LIB = $(LIBDIR)/libdespotify.la

CFLAGS += -I$(LIBDIR) -std=c99 
CFLAGS += `pkg-config --cflags gtk+-2.0 glib-2.0`
ifeq ($(MAEMO4),1)
 CFLAGS += -DMAEMO4 `pkg-config --cflags hildon-1`
 LDFLAGS += `pkg-config --libs hildon-1`
endif

LDFLAGS += `pkg-config --libs gtk+-2.0 glib-2.0`

all: maemify

# These are the files we depgen for. :-)
CFILES = $(OBJS:.o=.c)
include ../depgen.mk

maemify: $(OBJS) $(LIB)
	@echo LD $@
	$(SILENT)$(LT) --mode=link $(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(OBJS) $(LIB)

clean:
	$(LT) --mode=clean rm -f maemify
	rm -f $(OBJS) Makefile.dep

install: maemify
	@echo "Copying maemify binary to $(INSTALL_PREFIX)/bin/maemify"
	$(LT) --mode=install install maemify $(INSTALL_PREFIX)/bin/maemify

uninstall:
	@echo "Removing maemify..."
	rm -f $(INSTALL_PREFIX)/bin/maemify
