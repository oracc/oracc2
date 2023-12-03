#!/bin/dash

echo $0 $*

set 00lib/*.asl

if [ "$1" != "00lib/*.asl" ]; then
    for a in 00lib/*.asl ; do
	echo $0 checking signlist $a
	sx -c $a
    done
fi
