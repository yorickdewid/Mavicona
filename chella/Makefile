OBJS = chella.o nodemanager.o controlclient.o exec.o exec_cache.o exec_sql.o art.o sha1.o ../protoc/processjob.pb.o ../protoc/controlmessage.pb.o ../third_party/soci/build/lib/libsoci_core.a
CC = g++
DEBUG = -g -O0
LIBS = -lprotobuf -lzmq -pthread -ldl
CXXFLAGS = -Wall -std=c++11 -c $(DEBUG) -I.. -I../quid -I../third_party/upscaledb/include -I../third_party/soci
CFLAGS = -Wall -std=c++11 -c $(DEBUG) -I.. -I../quid -I../third_party/upscaledb/include
LFLAGS = -Wall $(DEBUG) $(LIBS) -L../third_party/soci/build/lib/

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o chella

dso:
	$(CC) -c -I. -fPIC dso_example.cpp -o dso_example.o
	$(CC) dso_example.o -shared -o libdso_example.so
	$(CC) -c -I. -fPIC dso_counter.cpp -o dso_counter.o
	$(CC) dso_counter.o -shared -o libdso_counter.so

jobify:
	$(CC) -Wall -std=c++11 -I../third_party/upscaledb/include -g -O0 jobify.cpp -ldl -o jobify

clean:
	$(RM) -rf *.o *.so
	$(RM) -rf chella jobify
