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

ifeq (,$(filter clean, $(MAKECMDGOALS))) # don't make deps for "make clean"
CFILES = $(filter-out %.a,$(DESPOTIFY_OBJS:.o=.c))

Makefile.dep:
	@echo Generating dependencies
	$(SILENT)$(CC) $(CFLAGS) -MM $(CFILES) > $@

-include Makefile.dep
endif

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
	@echo "Copying despotify binary to /usr/bin/despotify"
	$(LT) --mode=install install despotify /usr/bin/despotify

uninstall:
	@echo "Removing despotify..."
	rm -f /usr/bin/despotify
