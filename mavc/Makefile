OBJS = cynder.o pitcher.o chella.o quid.o server.o hbs.o main.o ../protoc/task.pb.o ../protoc/processjob.pb.o
DEBUG = -g -O0
LIBS = -lprotobuf -lzmq -L../quid -lquid
CXXFLAGS = -Wall -std=c++11 -c $(DEBUG) -I.. -I../quid
LFLAGS = -Wall $(DEBUG) $(LIBS)

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


all: $(OBJS)
	$(CC) $(CFLAGS) -c ../common/util.c -o util.o
	$(CXX) $(LFLAGS) util.o $(OBJS) -o mavc

clean:
	$(RM) -rf *.o *.so
	$(RM) -rf mavc
