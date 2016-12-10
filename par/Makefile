SOURCES = append.c block.c decode.c encode.c extract.c handle.c list.c util.c wrapper.c
OBJECTS = $(SOURCES:.c=.o)

all: module util

module: libmavpar.so

.c.o:
	$(CC) $(CFLAGS) -I ../compat -I../quid -g -O0 -fpic -DDEBUG -c $< -o $@

libmavpar.so: $(OBJECTS)
	$(CC) -shared $(OBJECTS) -o $@

util:
	$(CC) -Wall -Wl,-R. -I ../compat -I../quid -g -O0 libtar_hash.c \
		libtar_list.c par.c -o parutil -lmavpar -lz -lm -L. \
		../compat/dirname.c \
		../compat/basename.c \
		../compat/strmode.c \
		../compat/strlcpy.c \
		../quid/libquid.a

clean:
	$(RM) *.o *.so parutil
