OBJS = ../protoc/task.pb.o ../protoc/processjob.pb.o provision.o pitcher.o
CC = g++
DEBUG = -g -O0
LIBS = -lprotobuf -lzmq -lleveldb -lpthread
CXXFLAGS = -Wall -std=c++11 -c $(DEBUG) -I..
LFLAGS = -Wall $(DEBUG) $(LIBS)

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o pitcher

clean:
	$(RM) -rf *.o
	$(RM) -rf pitcher
