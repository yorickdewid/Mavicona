OBJS = cynder.o
CC = g++
DEBUG = -g
LIBS = 
CXXFLAGS = -Wall -std=c++11 -c $(DEBUG) -I.. -I../quid
LFLAGS = -Wall $(DEBUG) $(LIBS)

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o cynder

clean:
	$(RM) -rf *.o *.so
	$(RM) -rf cynder
