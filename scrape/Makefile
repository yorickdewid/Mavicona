OBJS = ../protoc/scrapedata.pb.o scrape.o ../quid/libquid.a
CC = g++
DEBUG = -g -O0
LIBS = -lprotobuf -lzmq $(shell python2.7-config --libs)
CXXFLAGS = -Wall -std=c++11 -c $(DEBUG) -I.. $(shell python2.7-config --includes) -I../quid
LFLAGS = -Wall $(DEBUG) $(LIBS)

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o scrape

dso:
	$(CC) -c -I. -fPIC example_ext.c -o example_ext.o
	$(CC) example_ext.o -shared -o libexample_ext.so

	$(CC) -c -I. -fPIC imap_ext.c -o imap_ext.o
	$(CC) -c -I. -fPIC ini.c -o ini.o
	$(CC) imap_ext.o ini.o -shared -lcurl -o libimap_ext.so

clean:
	$(RM) -rf *.o *.so
	$(RM) -rf scrape
