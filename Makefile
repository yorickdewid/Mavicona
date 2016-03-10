all:
	g++ extractor.cpp -lzmq -o extractor
	g++ scrape.cpp -lzmq -o scrape
