OBJS = chella.o nodemanager.o controlclient.o exec.o sha1.o ../protoc/processjob.pb.o ../protoc/controlmessage.pb.o
CC = g++
DEBUG = -g -O0
LIBS = -lprotobuf -lzmq -pthread -ldl
CXXFLAGS = -Wall -std=c++11 -c $(DEBUG) -I.. -I../quid -I../third_party/upscaledb/include
LFLAGS = -Wall $(DEBUG) $(LIBS)

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o chella

dso:
	$(CC) -c -I. -fPIC dso_example.cpp -o dso_example.o
	$(CC) dso_example.o -shared -o libdso_example.so

clean:
	$(RM) -rf *.o *.so
	$(RM) -rf chella
