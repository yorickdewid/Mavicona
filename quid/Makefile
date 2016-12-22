UTILDIR = util
COMPATDIR = ../compat
COMMONDIR = ../common
WFLAGS = -pedantic -std=c99 -Wall -Werror -Wextra -Winit-self -Wswitch-default -Wshadow
INCLUDES = -I. -I$(COMPATDIR) -I$(COMMONDIR)
CFLAGS = $(WFLAGS) $(INCLUDES) -DX64
LDFLAGS = -L.
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
DYNOBJ = libquid.so
STATOBJ = libquid.a

# Uncomment for debug mode
CFLAGS += -g -O0 -DDEBUG

ifeq ($(OS),Windows_NT)
	CFLAGS += -DWIN32
	ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
		CFLAGS += -DAMD64
	endif
	ifeq ($(PROCESSOR_ARCHITECTURE),x86)
		CFLAGS += -DIA32
	endif
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CFLAGS += -DLINUX
	endif
	ifeq ($(UNAME_S),Darwin)
		CFLAGS += -DOSX
	endif
	ifeq ($(UNAME_S),OpenBSD)
		CFLAGS += -DOBSD
	endif
	ifeq ($(UNAME_S),FreeBSD)
		CFLAGS += -DFBSD -I/usr/local/include
		LDFLAGS += -L/usr/local/lib
	endif
	UNAME_P := $(shell uname -p)
	ifeq ($(UNAME_P),x86_64)
		CFLAGS += -DAMD64
	endif
	ifneq ($(filter %86,$(UNAME_P)),)
		CFLAGS += -DIA32
	endif
	ifneq ($(filter arm%,$(UNAME_P)),)
		CFLAGS += -DARM
	endif
endif

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

#------------#
# Directives #
#------------#

.PHONY: all module util genquid clean cleanall cleanutil cleandist

all: module util

# Modules
module: $(DYNOBJ)

$(STATOBJ): $(OBJECTS)
	$(AR) rcs $@ $^

$(DYNOBJ): CFLAGS += -fPIC
$(DYNOBJ): $(OBJECTS)
	$(CC) -shared $(LDFLAGS) -o $@ $^ -lm

# Utils
util: genquid

genquid:
	$(CC) $(CFLAGS)	$(UTILDIR)/genquid.c $(LDFLAGS) -o $@ -lquid

# Cleanup
clean:
	@$(RM) -rf *.o *.so *.a

cleanall: clean cleanutil

cleanutil:
	@$(RM) -rf $(UTILDIR)/*.o
	@$(RM) -rf genquid

cleandist: clean
