OBJS = xRedisClient.o xRedisClient_connection.o xRedisClient_hashs.o xRedisClient_keys.o xRedisClient_lists.o xRedisClient_sets.o xRedisClient_sortedsets.o xRedisClient_strings.o xRedisPool.o
CC = g++
DEBUG = -g -O0
LIBS = -lpthread -lhiredis
CXXFLAGS = -Wall -std=c++11 -c -fPIC $(DEBUG)
LFLAGS = -Wall -shared $(DEBUG) $(LIBS)

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o libmavredis.so

clean:
	$(RM) -rf *.o *.so
