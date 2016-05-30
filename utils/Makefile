OBJS = cyndr.o main.o
CC = g++
DEBUG = -g
LIBS = 
CXXFLAGS = -Wall -std=c++11 -c $(DEBUG) -I..
LFLAGS = -Wall $(DEBUG) $(LIBS)

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o mav

clean:
	$(RM) -rf *.o *.so
	$(RM) -rf mav
