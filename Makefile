all:
	g++ extractor.cpp -lzmq -o extractor
	g++ scrape.cpp -lzmq $(shell python2.7-config --includes) $(shell python2.7-config --libs) -o scrape
