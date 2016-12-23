COMPATDIR = ../compat
COMMONDIR = ../common
WFLAGS = -pedantic -std=c99 -Wall -Werror -Wextra -Winit-self -Wswitch-default -Wshadow
#WFLAGS = 
INCLUDES = -I. -I$(COMPATDIR) -I$(COMMONDIR) $(shell python3-config --includes) -I../par -I../quid
CFLAGS = $(WFLAGS) $(INCLUDES) -DX64
LDFLAGS = -L. $(shell python3-config --libs) -L../par
SOURCES = package.c
OBJECTS = $(SOURCES:.c=.o)
# EXECOBJ = packer

# Uncomment for debug mode
CFLAGS += -g -O0 -DDEBUG

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

.PHONY: all util packer clean cleanall cleanutil cleandist

all: util

# Utils
util: packer

packer: $(OBJECTS)
	$(CC) $(CFLAGS)	$(COMPATDIR)/strlcpy.c -c -o strlcpy.o
	$(CC) $(CFLAGS)	$(COMMONDIR)/util.c -c -o util.o
	$(CC) $(CFLAGS)	packer.c strlcpy.o util.o $^ $(LDFLAGS) -o $@ -lmavpar

# Cleanup
clean:
	@$(RM) -rf *.o *.so *.a

cleanall: clean cleanutil

cleanutil:
	@$(RM) -rf packer

cleandist: clean
