UTILDIR = util
COMPATDIR = ../compat
COMMONDIR = ../common
WFLAGS = -pedantic -std=c99 -Wall -Werror -Wextra -Winit-self -Wswitch-default -Wshadow
INCLUDES = -I. -I$(COMPATDIR) -I$(COMMONDIR) -I../quid
CFLAGS = $(WFLAGS) $(INCLUDES) -DX64
LDFLAGS = -L. -L../quid
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
DYNOBJ = libmavpar.so
STATOBJ = libmavpar.a

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

.PHONY: all module util parutil clean cleanall cleanutil cleandist

all: module util

# Modules
module: $(DYNOBJ)

$(STATOBJ): $(OBJECTS)
	$(CC) $(CFLAGS)	$(COMPATDIR)/mkdirhier.c -c -o mkdirhier.o
	$(CC) $(CFLAGS)	$(COMPATDIR)/strmode.c -c -o strmode.o
	$(AR) rcs $@ mkdirhier.o strmode.o $^

$(DYNOBJ): CFLAGS += -fPIC
$(DYNOBJ): $(OBJECTS)
	$(CC) $(CFLAGS)	$(COMPATDIR)/mkdirhier.c -c -o mkdirhier.o
	$(CC) $(CFLAGS)	$(COMPATDIR)/strmode.c -c -o strmode.o
	$(CC) $(CFLAGS)	$(COMPATDIR)/strdup.c -c -o strdup.o
	$(CC) $(CFLAGS)	$(COMPATDIR)/strsep.c -c -o strsep.o
	$(CC) $(CFLAGS) $(COMPATDIR)/cdirname.c -c -o cdirname.o
	$(CC) -shared $(LDFLAGS) -o $@ mkdirhier.o strdup.o strsep.o strmode.o cdirname.o $^ -lquid -lz

# Utils
util: parutil

parutil:
	$(CC) $(CFLAGS)	$(COMPATDIR)/strlcpy.c -c -o $(UTILDIR)/strlcpy.o
	$(CC) $(CFLAGS)	$(UTILDIR)/parutil.c $(UTILDIR)/strlcpy.o $(LDFLAGS) -o $@ -lmavpar

# Cleanup
clean:
	@$(RM) -rf *.o *.so *.a

cleanall: clean cleanutil

cleanutil:
	@$(RM) -rf $(UTILDIR)/*.o
	@$(RM) -rf parutil

cleandist: clean
