OBJS = cynder.o engine.o data_index.o key_index.o text_index.o filepage.o ../protoc/storagequery.pb.o ../protoc/scrapedata.pb.o ../third_party/upscaledb/src/.libs/libupscaledb.a ../quid/libquid.a
CC = g++
DEBUG = -g -O0
LIBS = -lprotobuf -lzmq -lz -ldl -lboost_system -lboost_thread -lcrypto
CXXFLAGS = -Wall -std=c++11 -DZMQ_CPP11 -c $(DEBUG) -I.. -I../quid -I../third_party/upscaledb/include
LFLAGS = -Wall $(DEBUG) $(LIBS)

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o cynder

clean:
	$(RM) -rf *.o *.so
	$(RM) -rf cynder
