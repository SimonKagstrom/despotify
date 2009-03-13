#
# $Id$
# 

unexport LDFLAGS CFLAGS

LIBDIR = ../../lib
LIB = $(LIBDIR)/libdespotify.la

CFLAGS += -I$(LIBDIR)
LDFLAGS += -lncursesw

DESPOTIFY_OBJS = ADMclubbing.o despotify.o event.o ui-core.o ui-help.o ui-player.o ui-playlist.o ui-search.o

ifneq ($(NOGUI), 1)
    CFLAGS += -DGUI
endif

.PHONY: all clean install uninstall
all: despotify 

# These are the files we depgen for. :-)
CFILES = $(DESPOTIFY_OBJS:.o=.c)
include ../depgen.mk

%.o: %.c
	@echo CC $<
	$(SILENT)$(CC) $(CFLAGS) -o $@ -c $<

despotify: $(DESPOTIFY_OBJS) $(LIB)
	@echo LD $@
	$(SILENT)$(LT) --mode=link $(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(LIB) $(DESPOTIFY_OBJS)

clean:
	$(LT) --mode=clean rm -f despotify
	rm -f $(DESPOTIFY_OBJS) Makefile.dep

install: despotify 
	@echo "Copying despotify binary to $(INSTALL_PREFIX)/bin/despotify"
	$(LT) --mode=install install despotify $(INSTALL_PREFIX)/bin/despotify

uninstall:
	@echo "Removing despotify..."
	rm -f $(INSTALL_PREFIX)/bin/despotify
