all:
	g++ -std=c++11 config.cpp extractor.cpp -lzmq -o extractor
	g++ -std=c++11 config.cpp scrape.cpp -lzmq $(shell python2.7-config --includes) $(shell python2.7-config --libs) -o scrape
