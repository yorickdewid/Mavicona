OBJS = ../protoc/scrapedata.pb.o scrape.o ../quid/libquid.a
DEBUG = -g -O0
LIBS = -lprotobuf -lzmq $(shell python2.7-config --libs) -lcurl
CXXFLAGS = -Wall -std=c++11 -c $(DEBUG) -DLIBPYTHON -I.. $(shell python2.7-config --includes) -I../quid -I../third_party/lavril
LFLAGS = -Wall $(DEBUG) $(LIBS)

all: $(OBJS)
	$(CXX) $(LFLAGS) $(OBJS) -o scrape

dso:
	$(CC) -c -I. -fPIC example_ext.c -o example_ext.o
	$(CC) example_ext.o -shared -o libexample_ext.so

	$(CC) -c -I. -fPIC imap_ext.c -o imap_ext.o
	$(CC) -c -I. -fPIC ini.c -o ini.o
	$(CC) imap_ext.o ini.o -shared -lcurl -o libimap_ext.so

static:
	$(CC) -I. -c example_ext.c -o example_ext.o
	$(CC) -Wall -std=c++11 -c $(DEBUG) -DSTATIC -I. -I.. -I../quid -c -o scrape.o scrape.cpp
	$(CC) -Wall $(DEBUG) -lprotobuf -lzmq ../protoc/scrapedata.pb.o  scrape.o ../quid/libquid.a example_ext.o -o scrape_static

clean:
	$(RM) -rf *.o *.so
	$(RM) -rf scrape
	$(RM) -rf scrape_static
