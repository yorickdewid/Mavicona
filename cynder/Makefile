OBJS = cynder.o ../third_party/upscaledb/src/.libs/libupscaledb.a
CC = g++
DEBUG = -g
LIBS = -lprotobuf -lzmq -lz -lz -ldl -lboost_system -lboost_thread -lcrypto
CXXFLAGS = -Wall -std=c++11 -c $(DEBUG) -I.. -I../quid -I../third_party/upscaledb/include
LFLAGS = -Wall $(DEBUG) $(LIBS)

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o cynder

clean:
	$(RM) -rf *.o *.so
	$(RM) -rf cynder