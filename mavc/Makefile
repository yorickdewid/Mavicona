OBJS = cynder.o pitcher.o hbs.o main.o ../protoc/task.pb.o
CC = g++
DEBUG = -g -O0
LIBS = -lprotobuf -lzmq
CXXFLAGS = -Wall -std=c++11 -c $(DEBUG) -I.. 
LFLAGS = -Wall $(DEBUG) $(LIBS)

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o mavc

clean:
	$(RM) -rf *.o *.so
	$(RM) -rf mavc
