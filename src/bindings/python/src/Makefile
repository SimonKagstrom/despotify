SRCS = spytify.pyx
OUTS := $(SRCS:.pyx=.c)

CYTHON = cython

spytify.c: despotify.pxd

%.c: %.pyx
	$(CYTHON) `pkg-config --cflags-only-I despotify` -o $@ $< 

.PHONY: all clean

all: $(OUTS)

clean:
	$(RM) $(OUTS)
