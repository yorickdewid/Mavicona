OBJS = chella.o ../protoc/processjob.pb.o
CC = g++
DEBUG = -g -O0
LIBS = -lprotobuf -lzmq
CXXFLAGS = -Wall -std=c++11 -c $(DEBUG) -I.. -I../quid -I../third_party/upscaledb/include
LFLAGS = -Wall $(DEBUG) $(LIBS)

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o chella

clean:
	$(RM) -rf *.o *.so
	$(RM) -rf chella
