OBJS = ../common/config.o ../protoc/scrapedata.pb.o rule_scan.o rule_parse.o ruler.o detect.o extractor.o
CC = g++
LEX = flex
YACC = bison
DEBUG = -g
LIBS = -lprotobuf -lzmq -lmagic
CXXFLAGS = -Wall -std=c++11 -c $(DEBUG) -I.. 
LFLAGS = -Wall $(DEBUG) $(LIBS)

all: parse $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o extractor

parse:
	$(LEX) -o rule_scan.cpp --header-file=rule_scan.h rule_scan.l
	$(YACC) -o rule_parse.cpp --defines=rule_parse.h rule_parse.y

clean:
	$(RM) -rf *.o
	$(RM) -rf extractor