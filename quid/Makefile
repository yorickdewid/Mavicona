UTILDIR = util
WFLAGS = -pedantic -std=c99 -Wall -Werror -Wextra -Winit-self -Wswitch-default -Wshadow
CFLAGS = $(WFLAGS) -DX64
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
DYNOBJ = libquid.so
STATOBJ = libquid.a

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
		CFLAGS += -DFBSD -I/usr/local/include -L/usr/local/lib
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

.PHONY: all genquid clean cleanutil cleandist

all: module util

# Modules
module: CFLAGS += -g -O0 -DDEBUG -DMOD
module: $(DYNOBJ)

$(STATOBJ): $(OBJECTS)
	$(AR) rcs $@ $^

$(DYNOBJ): CFLAGS += -fPIC
$(DYNOBJ): $(OBJECTS)
	$(CC) -shared -o $@ $^

# Utils
util: genquid

genquid:
	$(CC) $(CFLAGS)	$(UTILDIR)/genquid.c $(LDFLAGS) -o $@ -L. -lquid -lm

# Cleanup
cleanall: clean cleanutil

clean:
	@$(RM) -rf *.o *.so *.a

cleanutil:
	@$(RM) -rf $(UTILDIR)/*.o
	@$(RM) -rf genquid

cleandist: clean
