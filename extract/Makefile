OBJS = ../protoc/scrapedata.pb.o ../protoc/task.pb.o ../third_party/lodepng/lodepng.o rule_scan.o rule_parse.o plaintext_handler.o png_handler.o action_log.o action_queue.o action_store.o action_discard.o ruler.o detect.o extractor.o
CC = g++
LEX = flex
YACC = bison
DEBUG = -g -O0
LIBS = -lprotobuf -lzmq -lmagic
CXXFLAGS = -Wall -std=c++11 -c $(DEBUG) -I..  -I../third_party
LFLAGS = -Wall $(DEBUG) $(LIBS)

all: parse $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o extractor

parse:
	$(LEX) -o rule_scan.cpp --header-file=rule_scan.h rule_scan.l
	$(YACC) -o rule_parse.cpp --defines=rule_parse.h rule_parse.y

ruler: parse
	$(CC) -std=c++11 -DRULER rule_parse.cpp rule_scan.cpp -o ruler

clean:
	$(RM) -rf *.o
	$(RM) -rf ruler
	$(RM) -rf extractor
