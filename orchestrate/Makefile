OBJS = client.o error_page.o file.o header.o http.o orchestrator.o server.o socket.o thread.o ../quid/libquid.a
CC = g++
DEBUG = -g -O0
LIBS = -lpthread
CXXFLAGS = -Wall -std=c++11 -c $(DEBUG) -I.. -I../quid
LFLAGS = -Wall $(DEBUG) $(LIBS)

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o orchestrator

clean:
	$(RM) -rf *.o *.so
	$(RM) -rf orchestrator
