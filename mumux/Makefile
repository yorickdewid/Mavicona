UTILDIR = util
COMPATDIR = ../compat
COMMONDIR = ../common
WFLAGS = -pedantic -std=c99 -Wall -Werror -Wextra -Winit-self -Wswitch-default -Wshadow
INCLUDES = -I. -I$(COMPATDIR) -I$(COMMONDIR)
CFLAGS = $(WFLAGS) $(INCLUDES) -DX64
LDFLAGS = -L.
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
DYNOBJ = libmumux.so
STATOBJ = libmumux.a

# Uncomment for debug mode
#CFLAGS += -g -O0 -DDEBUG

ifeq ($(OS),Windows_NT)
	CFLAGS += -DWIN32
	CXXFLAGS += -DWIN32
	ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
		CFLAGS += -DAMD64
		CXXFLAGS += -DAMD64
	endif
	ifeq ($(PROCESSOR_ARCHITECTURE),x86)
		CFLAGS += -DIA32
		CXXFLAGS += -DIA32
	endif
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CFLAGS += -DLINUX
		CXXFLAGS += -DLINUX
	endif
	ifeq ($(UNAME_S),Darwin)
		CFLAGS += -DOSX
		CXXFLAGS += -DOSX
	endif
	ifeq ($(UNAME_S),OpenBSD)
		CFLAGS += -DOBSD
		CXXFLAGS += -DOBSD
	endif
	ifeq ($(UNAME_S),FreeBSD)
		CFLAGS += -DFBSD -I/usr/local/include
		CXXFLAGS += -DFBSD -I/usr/local/include
		LDFLAGS += -L/usr/local/lib
	endif
	UNAME_P := $(shell uname -p)
	ifeq ($(UNAME_P),x86_64)
		CFLAGS += -DAMD64
		CXXFLAGS += -DAMD64
	endif
	ifneq ($(filter %86,$(UNAME_P)),)
		CFLAGS += -DIA32
		CXXFLAGS += -DIA32
	endif
	ifneq ($(filter arm%,$(UNAME_P)),)
		CFLAGS += -DARM
		CXXFLAGS += -DARM
	endif
endif

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

#------------#
# Directives #
#------------#

.PHONY: all module util httputil clean cleanall cleanutil cleandist

# all:
# 	$(CC) -I. base64.c connection.c request.c response.c server.c sha.c socket.c url.c util.c util/httputil.c -o xyz
all: module util

# Modules
module: $(DYNOBJ)

$(STATOBJ): $(OBJECTS)
	$(AR) rcs $@ $^

$(DYNOBJ): CFLAGS += -fPIC
$(DYNOBJ): $(OBJECTS)
	$(CC) -shared $(LDFLAGS) -o $@ $^

# Utils
util: httputil

httputil:
	$(CC) $(CFLAGS)	$(UTILDIR)/httputil.c $(LDFLAGS) -o $@ -lmumux

# Cleanup
clean:
	@$(RM) -rf *.o *.so *.a

cleanall: clean cleanutil

cleanutil:
	@$(RM) -rf $(UTILDIR)/*.o
	@$(RM) -rf httputil

cleandist: clean
