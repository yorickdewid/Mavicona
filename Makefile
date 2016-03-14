all:
	g++ -std=c++11 config.cpp extractor.cpp -lzmq -o extractor
	g++ -std=c++11 config.cpp scrape.cpp -lzmq $(shell python2.7-config --includes) $(shell python2.7-config --libs) -o scrape

dso:
	gcc -c -fPIC example_ext.c -o example_ext.o
	gcc example_ext.o -shared -o libexample_ext.so
