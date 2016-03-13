all:
	g++ config.cpp extractor.cpp -lzmq -o extractor
	g++ config.cpp scrape.cpp -lzmq $(shell python2.7-config --includes) $(shell python2.7-config --libs) -o scrape
