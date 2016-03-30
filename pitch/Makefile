OBJS = queue.o pitcher.o
CC = g++
DEBUG = -g
LIBS = -lprotobuf -lzmq -lleveldb
CXXFLAGS = -Wall -std=c++11 -c $(DEBUG)
LFLAGS = -Wall $(DEBUG) $(LIBS)

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o pitcher

clean:
	$(RM) -rf *.o
	$(RM) -rf pitcher
