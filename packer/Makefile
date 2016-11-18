all:
	$(CC) packer.c $(shell python3-config --includes) -o jobpacker -ltar $(shell python3-config --libs)
clean:
	$(RM) jobpacker