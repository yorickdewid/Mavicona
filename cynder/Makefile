OBJS = cynder.o \
		engine.o \
		data_index.o \
		key_index.o \
		text_index.o \
		filepage.o \
		../protoc/storagequery.pb.o \
		../protoc/scrapedata.pb.o \
		../third_party/upscaledb/src/.libs/libupscaledb.a
DEBUG = -g -O0
LIBS = -lprotobuf -lzmq -lz -ldl -lboost_system -lboost_thread -lcrypto -L../quid -lquid
CXXFLAGS = -Wall -std=c++11 -DZMQ_CPP11 -c $(DEBUG) -I.. -I../quid -I../third_party/upscaledb/include
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

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

all: $(OBJS)
	$(CC) $(CFLAGS) -c ../common/util.c -o util.o
	$(CXX) $(OBJS) util.o $(LFLAGS) -o cynder

clean:
	$(RM) -rf *.o *.so
	$(RM) -rf cynder
