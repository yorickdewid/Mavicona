AR = ar
UTILDIR = util
WFLAGS = -pedantic-errors -std=gnu99 -Wall -Werror -Wextra -Winit-self -Wswitch-default -Wshadow $(shell python3-config --includes)
CFLAGS = $(WFLAGS) -DX64 -g -O0 -DDEBUG -I../par -I../quid -I../compat -I../common -I../quid -L../par -L../quid
SOURCES = package.c ../compat/strlcpy.c ../common/util.c
CLIENT_SOURCES = packer.c
OBJECTS = $(SOURCES:.c=.o)
CLIENTOBJECTS = $(SOURCES:.c=.o) $(CLIENT_SOURCES:.c=.o)
EXECUTABLECLIENT = packer

all: packer

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

packer:
	$(CC) $(CFLAGS) $(SOURCES) $(CLIENT_SOURCES) -o $(EXECUTABLECLIENT) $(shell python3-config --libs) -lmavpar -lz -lquid

clean:
	$(RM) -rf *.o
	$(RM) -rf packer