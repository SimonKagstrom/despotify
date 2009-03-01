#
# $Id$
# 

unexport LDFLAGS CFLAGS

LIBDIR = ../../lib

CFLAGS += -I$(LIBDIR)
LDFLAGS += -lncurses $(LIBDIR)/libdespotify.la

DESPOTIFY_OBJS = ADMclubbing.o despotify.o event.o ui-core.o ui-help.o ui-player.o ui-playlist.o ui-search.o

ifneq ($(NOGUI) , 1)
    CFLAGS += -DGUI
endif

all: despotify 

.PHONY: all clean lib install uninstall

ifeq (,$(filter clean, $(MAKECMDGOALS))) # don't make deps for "make clean"
CFILES = $(filter-out %.a,$(DESPOTIFY_OBJS:.o=.c))

Makefile.dep:
	$(CC) $(CFLAGS) -MM $(CFILES) > $@

-include Makefile.dep
endif

clean:
	$(LT) --mode=clean rm -f despotify
	rm -f $(DESPOTIFY_OBJS) Makefile.dep

despotify: $(DESPOTIFY_OBJS) lib
	$(LT) --mode=link $(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(DESPOTIFY_OBJS)

lib:
	$(MAKE) -C $(LIBDIR)

install: despotify 
	@echo "Copying despotify binary to /usr/bin/despotify"
	$(LT) --mode=install install despotify /usr/bin/despotify

uninstall:
	@echo "Removing despotify..."
	rm -f /usr/bin/despotify
