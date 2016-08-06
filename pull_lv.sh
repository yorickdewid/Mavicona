#!/bin/bash
#
# Pull Lavril

if [ ! -d "../lavril" ]; then
  echo "Nope"
fi

if [ ! -d "third_party/lavril" ]; then
  mkdir -f third_party/lavril
 fi

cp -r ../lavril/lib/*  third_party/lavril/
cp -r ../lavril/include/*  third_party/lavril/