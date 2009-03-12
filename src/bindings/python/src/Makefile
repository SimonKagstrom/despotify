SRCS = spytify.pyx

OUTS := $(SRCS:.pyx=.c)

CYTHON = cython

all: $(OUTS)

spytify.c: despotify.pxd playlist.pxd playlist.pxi

%.c: %.pyx
	$(CYTHON) `pkg-config --cflags-only-I despotify` -o $@ $< 

.PHONY: all clean

clean:
	$(RM) $(OUTS)
