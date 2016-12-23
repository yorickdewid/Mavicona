#!/bin/sh

test_1() {
	echo "one" > one.txt
	echo "two" > two.txt
	echo "tree" > tree.txt

	./parutil -c testcase.par one.txt two.txt tree.txt
	./parutil -l testcase.par
	rm one.txt two.txt tree.txt
	./parutil -x testcase.par
	rm testcase.par

	cat one.txt
	cat two.txt
	cat tree.txt

	rm one.txt
	rm two.txt
	rm tree.txt
}

test_2() {
	mkdir _xtmp_
	echo "Some random string of data" > rand.txt
	echo "data in dir" > _xtmp_/data.txt
	echo "hsbcydyu,cbeklgf7igf23tr87o234tr87owetr7oetrfwei" > bin.exe

	./parutil -c testcase.par rand.txt _xtmp_/data.txt bin.exe
	./parutil -l testcase.par
	rm rand.txt _xtmp_/data.txt bin.exe
	./parutil -x testcase.par
	rm testcase.par

	cat rand.txt
	cat _xtmp_/data.txt
	cat bin.exe

	rm rand.txt
	rm _xtmp_/data.txt
	rm bin.exe
	rm -r _xtmp_
}

test_1
test_2
