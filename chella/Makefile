OBJS = chella.o nodemanager.o controlclient.o archive.o wal.o exec.o exec_cache.o exec_sql.o sha1.o ../protoc/processjob.pb.o ../protoc/controlmessage.pb.o ../quid/libquid.a ../packer/libmpkg.a
CC = g++
DEBUG = -g -O0
LIBS = -lprotobuf -lzmq $(shell python3-config --libs) -ltar -lz -lleveldb
CXXFLAGS = -Wall -std=c++11 -c $(DEBUG) -DCACHE -DZMQ_CPP11 -I.. -I../quid -I../packer -I../third_party/upscaledb/include -I../third_party/soci $(shell python3-config --includes) -Wno-write-strings
CFLAGS = -Wall -std=c++11 -c $(DEBUG) -I.. -I../quid -I../packer -I../third_party/upscaledb/include
LFLAGS = -Wall $(DEBUG) $(LIBS) -L../third_party/soci/build/lib/

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o chella

clean:
	$(RM) -rf *.o *.so
	$(RM) -rf chella
