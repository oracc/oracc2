#!/bin/sh
cut -d' ' -f3 gdltokens.y | sed 's/^/#define /' >gdltoksub.h
while IFS="" read line
do
    if [ "$line" = "###gdltokens.y###" ]; then
	cat gdltokens.y
    else
	printf "%s\n" "$line"
    fi
done  <gdl.yy >gdl.y
