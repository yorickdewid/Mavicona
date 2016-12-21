#!/bin/bash

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
