OBJS = ../common/config.o ../protoc/scrapedata.pb.o detect.o extractor.o
CC = g++
LEX = flex
YACC = bison
DEBUG = -g
LIBS = -lprotobuf -lzmq -lmagic
CXXFLAGS = -Wall -std=c++11 -c $(DEBUG) -I.. 
LFLAGS = -Wall $(DEBUG) $(LIBS)

all: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o extractor

ruler:
	$(LEX) -o rule_scan.c rule_scan.l
	$(YACC) -d rule_parse.y
	$(CC) rule_scan.c rule_parse.tab.c -o ruler

clean:
	$(RM) -rf *.o
	$(RM) -rf extractor
	$(RM) -rf ruler
