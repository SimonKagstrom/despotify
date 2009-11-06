package?=despotify

OBJS = simple.o
LIBDIR = ../../lib
LIB = $(LIBDIR)/libdespotify.la

CFLAGS += -I$(LIBDIR)

ifeq ($(LINUX_BACKEND),libao)
 OBJS += libao.o
 LDFLAGS += -lao
endif


all: simple

# These are the files we depgen for. :-)
CFILES = $(OBJS:.o=.c)
include ../depgen.mk

simple: $(OBJS) $(LIB)
	@echo LD $@
	$(SILENT)$(LT) --mode=link $(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(OBJS) $(LIB)

clean:
	$(LT) --mode=clean rm -f simple
	rm -f $(OBJS) Makefile.dep

install: simple
	@echo "Copying simple binary to $(INSTALL_PREFIX)/bin/${package}-simple"
	install -d $(INSTALL_PREFIX)/bin/
	$(LT) --mode=install install simple $(INSTALL_PREFIX)/bin/${package}-simple

uninstall:
	@echo "Removing simple..."
	rm -f $(INSTALL_PREFIX)/bin/${package}-simple
