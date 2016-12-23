OBJS = chella.o \
		nodemanager.o \
		controlclient.o \
		ace/config.o \
		ace/db.o \
		ace/ipc.o \
		localenv.o \
		package.o \
		wal.o \
		exec.o \
		sha1.o \
		../protoc/processjob.pb.o \
		../protoc/controlmessage.pb.o
CXX = g++
DEBUG = -g -O0
LIBS = -lprotobuf -lzmq $(shell python3-config --libs) -lleveldb -L../par -lmavpar -L../quid -lquid
CXXFLAGS = -Wall -std=c++11 -c $(DEBUG) -DDEBUG -DZMQ_CPP11 -I.. -I../common -I../quid -I../par -I../compat -I../packer -I../third_party $(shell python3-config --includes) -Wno-write-strings
LXXFLAGS = -Wall $(DEBUG) $(LIBS)

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

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

all: $(OBJS)
	$(CC) $(CFLAGS) -c ../common/util.c -o util.o
	$(CC) $(CFLAGS) -c ../compat/strlcpy.c -o strlcpy.o
	$(CXX) $(OBJS) util.o strlcpy.o $(LXXFLAGS) -o chella

clean:
	$(RM) -rf *.o ace/*.o
	$(RM) -rf chella
