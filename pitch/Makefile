OBJS = ../protoc/task.pb.o provision.o pitcher.o
CC = g++
DEBUG = -g
LIBS = -lprotobuf -lzmq -lleveldb -lpthread
CXXFLAGS = -Wall -std=c++11 -c $(DEBUG) -I..
LFLAGS = -Wall $(DEBUG) $(LIBS)

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o pitcher

clean:
	$(RM) -rf *.o
	$(RM) -rf pitcher
