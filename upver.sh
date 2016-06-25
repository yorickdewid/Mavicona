#!/bin/bash
#
# Inject version into module header

FILE=common/module.h
DATE=`date +%Y-%m-%d:%H:%M:%S`

echo -n "New version: "
read nversion

if [ -z "$nversion" ]; then
	nversion=`git describe --always`
fi

sed -i '/#define MAV_VERSION/c\#define MAV_VERSION\t"'$nversion'"' $FILE
sed -i '/#define MAV_BUILD/c\#define MAV_BUILD\t"'$DATE'"' $FILE
