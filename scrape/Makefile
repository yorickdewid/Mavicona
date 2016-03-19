OBJS = ../common/config.o ../protoc/scrapedata.pb.o scrape.o
CC = g++
DEBUG = -g
LIBS = -lprotobuf -lzmq $(shell python2.7-config --libs)
CXXFLAGS = -Wall -std=c++11 -c $(DEBUG) -I.. $(shell python2.7-config --includes)
LFLAGS = -Wall $(DEBUG) $(LIBS)

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o scrape

dso:
	$(CC) -c -fPIC example_ext.c -o example_ext.o
	$(CC) example_ext.o -shared -o libexample_ext.so

clean:
	$(RM) -rf *.o *.so
	$(RM) -rf scrape
