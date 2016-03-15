all:
	protoc --cpp_out=. scrapedata.proto
	g++ -std=c++11 -g config.cpp scrapedata.pb.cc extractor.cpp -lprotobuf -lzmq -o extractor
	g++ -std=c++11 -g config.cpp scrapedata.pb.cc scrape.cpp -lprotobuf -lzmq $(shell python2.7-config --includes) $(shell python2.7-config --libs) -o scrape

dso:
	gcc -c -fPIC example_ext.c -o example_ext.o
	gcc example_ext.o -shared -o libexample_ext.so
