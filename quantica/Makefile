INCLUDE = include
UTILDIR = util
VALGRIND = valgrind
CPPCHECK = cppcheck
CPPCHECKFLAGS = --quiet --std=c1x
VALFLAGS = --leak-check=full --track-origins=yes --show-reachable=yes
WFLAGS = -pedantic-errors -std=c1x -Wall -Werror -Wextra -Winit-self -Wswitch-default -Wshadow
CFLAGS = $(WFLAGS) -DX64 -DTN12
LDFLAGS = -lm
SOURCES = $(wildcard *.c)
CLIENT_SOURCES = $(UTILDIR)/qcli.c
OBJECTS = $(SOURCES:.c=.o)
CLIENTOBJECTS = $(SOURCES:.c=.o) $(CLIENT_SOURCES:.c=.o)
EXECUTABLE = quantica
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

.PHONY: all debug test memcheck cov fixeof genquid verminor genlookup3 qcli clean cleandb cleanutil cleandist

all: module

debug: CFLAGS += -g -O0 -DDEBUG -DRESOLV -DDAEMON
debug: $(EXECUTABLE)

release: CFLAGS += -O2 -DDAEMON
release: $(EXECUTABLE)

module: CFLAGS += -g -O0 -DDEBUG -DRESOLV -DMOD
module: libquant.a

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(BINDIR)/$@

.c.o:
	$(CC) -I$(INCLUDE) $(CFLAGS) -c $< -o $@

libquant.a: $(OBJECTS)
	ar rcs $@ $^

memcheck: debug
	cd $(BINDIR) && $(VALGRIND) $(VALFLAGS) ./$(EXECUTABLE) -f

util: fixeof genquid verminor genlookup3 qcli

fixeof:
	$(CC) $(UTILDIR)/lfeof.c -pedantic-errors -std=c1x -Wall -Werror -Wextra -Winit-self -Wswitch-default -Wshadow -o $(UTILDIR)/lfeof
	find . -type f -name *.[c\|h] -print -exec $(UTILDIR)/lfeof {} \;

genquid:
	$(CC) $(CFLAGS) -I$(INCLUDE) \
		$(SRCDIR)/time.c \
		$(SRCDIR)/log.c \
		$(SRCDIR)/error.c \
		$(SRCDIR)/zmalloc.c \
		$(SRCDIR)/itoa.c \
		$(SRCDIR)/quid.c \
		$(SRCDIR)/arc4random.c \
		$(UTILDIR)/genquid.c $(LDFLAGS) -o $(UTILDIR)/genquid

genlookup3:
	$(CC) $(CFLAGS) -Wswitch-default -Wshadow -I$(INCLUDE) $(SRCDIR)/time.c $(SRCDIR)/log.c $(SRCDIR)/jenhash.c $(SRCDIR)/arc4random.c $(UTILDIR)/genlookup3.c $(LDFLAGS) -o $(UTILDIR)/genlookup3

qcli: CFLAGS += -DCLIENT
qcli: $(CLIENTOBJECTS)
	$(CC) -I$(INCLUDE) $(CFLAGS) $(CLIENTOBJECTS) $(LDFLAGS) -o $(UTILDIR)/$@

cleanall: clean cleandb cleanutil

clean:
	@$(RM) -rf *.o
	@$(RM) -rf $(TESTDIR)/*.o

cleandb:
	@$(RM) -rf $(BINDIR)/*

cleanutil:
	@$(RM) -rf $(UTILDIR)/*.o
	@$(RM) -rf $(UTILDIR)/lfeof
	@$(RM) -rf $(UTILDIR)/genquid
	@$(RM) -rf $(UTILDIR)/genlookup3
	@$(RM) -rf $(UTILDIR)/qcli

cleandist: clean
