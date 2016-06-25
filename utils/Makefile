OBJS = cynder.o main.o
CC = g++
DEBUG = -g -O0
LIBS = 
CXXFLAGS = -Wall -std=c++11 -c $(DEBUG) -I.. 
LFLAGS = -Wall $(DEBUG) $(LIBS)

all: $(OBJS) genquid
	$(CC) $(LFLAGS) $(OBJS) -o mav

genquid:
	cc -Wall -I../quid genquid.c ../quid/libquid.a -lm -o genquid

clean:
	$(RM) -rf *.o *.so
	$(RM) -rf mav
	$(RM) -rf genquid
