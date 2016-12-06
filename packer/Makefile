AR = ar
UTILDIR = util
WFLAGS = -pedantic-errors -std=gnu99 -Wall -Werror -Wextra -Winit-self -Wswitch-default -Wshadow $(shell python3-config --includes)
CFLAGS = $(WFLAGS) -DX64
SOURCES = package.c
CLIENT_SOURCES = packer.c
OBJECTS = $(SOURCES:.c=.o)
CLIENTOBJECTS = $(SOURCES:.c=.o) $(CLIENT_SOURCES:.c=.o)
EXECUTABLECLIENT = jobpacker

all: module jobpacker

module: CFLAGS += -g -O0 -DDEBUG
module: libmpkg.a

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

libmpkg.a: $(OBJECTS)
	$(AR) rcs $@ $^

jobpacker: module
	$(CC) $(CFLAGS) $(CLIENT_SOURCES) libmpkg.a -o $(EXECUTABLECLIENT) $(shell python3-config --libs) -ltar -lz

clean:
	$(RM) -rf libmpkg.a *.o
	$(RM) -rf jobpacker