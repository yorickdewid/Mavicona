all: genquid

genquid:
	cc -Wall -I../quid genquid.c ../quid/libquid.a -lm -o genquid

clean:
	$(RM) -rf *.o *.so
	$(RM) -rf genquid
