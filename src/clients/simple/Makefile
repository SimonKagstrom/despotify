OBJS = simple.o
CFLAGS = -Wall -Wextra -I../../lib -std=c99
DLIB = ../../lib/.libs/libdespotify.a
LIBS =  -lz -lexpat -lvorbisfile -lcrypto -lresolv 

simple: $(OBJS) $(DLIB)
	$(CC) -o $@ $(OBJS) $(DLIB) $(LIBS)

lib:
	cd ../.. && make
