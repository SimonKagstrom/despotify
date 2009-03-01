GSTAPP_OBJS = gstappsrc.o gstappbuffer.o gstapp-marshal.o

glib_enum_prefix = gst_app

all: libgstapp.a

libgstapp.a: $(GSTAPP_OBJS)
	$(AR) rcs $@ $^

gstapp-marshal.o: gstapp-marshal.h

gstappsrc.o: gstapp-marshal.h

gstappbuffer.o: gstapp-marshal.h

clean:
	rm -f *.a *.o gstapp-marshal.c gstapp-marshal.h

%-marshal.h: %-marshal.list
	glib-genmarshal --header --prefix=$(glib_enum_prefix)_marshal $^ > $*-marshal.h.tmp
	mv $*-marshal.h.tmp $*-marshal.h

%-marshal.c: %-marshal.list
	echo "#include \"$*-marshal.h\"" >> $*-marshal.c.tmp
	glib-genmarshal --body --prefix=$(glib_enum_prefix)_marshal $^ >> $*-marshal.c.tmp
	mv $*-marshal.c.tmp $*-marshal.c
