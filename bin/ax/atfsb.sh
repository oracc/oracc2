#!/bin/sh
#
# Pipeline to add sentence boundaries to lemmatized ATF files.
#
# Output is in a subdirectory 'x' which is created if it doesn't
# exist.
#
if [ ! -d x ]; then
    mkdir -p x
    if [ ! -d x ]; then
	echo $0: unable to create output directory 'x'. Stop.
	exit 1
    fi
fi
for a in *.atf ; do
    atfsb-one.sh $a
done
