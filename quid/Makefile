AR = ar
UTILDIR = util
WFLAGS = -pedantic-errors -std=c99 -Wall -Werror -Wextra -Winit-self -Wswitch-default -Wshadow
CFLAGS = $(WFLAGS) -DX64 -DTN12
SOURCES = $(wildcard *.c)
CLIENT_SOURCES = $(UTILDIR)/qcli.c
OBJECTS = $(SOURCES:.c=.o)
CLIENTOBJECTS = $(SOURCES:.c=.o) $(CLIENT_SOURCES:.c=.o)
EXECUTABLECLIENT = qcli

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
		LDFLAGS += -lrt
	endif
	ifeq ($(UNAME_S),Darwin)
		CFLAGS += -DOSX
	endif
	ifeq ($(UNAME_S),OpenBSD)
		CFLAGS += -DOBSD
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

.PHONY: all fixeof genquid genlookup3 qcli clean cleanutil cleandist

all: module

module: CFLAGS += -g -O0 -DDEBUG -DMOD
module: libquid.a

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

libquid.a: $(OBJECTS)
	$(AR) rcs $@ $^

util: fixeof genquid genlookup3 qcli

fixeof:
	$(CC) $(UTILDIR)/lfeof.c -pedantic-errors -std=c1x -Wall -Werror -Wextra -Winit-self -Wswitch-default -Wshadow -o $(UTILDIR)/lfeof
	find . -type f -name *.[c\|h] -print -exec $(UTILDIR)/lfeof {} \;

genquid:
	$(CC) $(CFLAGS) \
		time.c \
		log.c \
		error.c \
		zmalloc.c \
		itoa.c \
		quid.c \
		arc4random.c \
		$(UTILDIR)/genquid.c $(LDFLAGS) -o genquid

cleanall: clean cleanutil

clean:
	@$(RM) -rf *.o

cleanutil:
	@$(RM) -rf $(UTILDIR)/*.o
	@$(RM) -rf $(UTILDIR)/lfeof
	@$(RM) -rf $(UTILDIR)/genquid
	@$(RM) -rf $(UTILDIR)/genlookup3
	@$(RM) -rf $(UTILDIR)/qcli

cleandist: clean
