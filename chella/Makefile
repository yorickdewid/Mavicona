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
OBJS += ../quid/libquid.a ../common/util.o ../compat/strlcpy.o
DEBUG = -g -O0
LIBS = -lprotobuf -lzmq $(shell python3-config --libs) -lleveldb -L../par -lmavpar -L../quid -lquid
CXXFLAGS = -Wall -std=c++11 -c $(DEBUG) -DDEBUG -DZMQ_CPP11 -I.. -I../common -I../quid -I../par -I../compat -I../packer -I../third_party $(shell python3-config --includes) -Wno-write-strings
LXXFLAGS = -Wall $(DEBUG) $(LIBS)

all: $(OBJS)
	$(CXX) $(OBJS) $(LXXFLAGS) -o chella

clean:
	$(RM) -rf *.o ace/*.o
	$(RM) -rf chella
