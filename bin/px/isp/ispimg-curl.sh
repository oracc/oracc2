#!/bin/sh
#
# Helper for ispimg.sh:
#
# $1 = PX number
# $2 = 1 if use_eic was set in parent script ispimg.sh
#
# Check imgs with curl and retrieve if caching
#
px=$1
use_eic=$2
cdli='https://cdli.mpiwg-berlin.mpg.de/';
photo='dl/photo/';
line='dl/lineart/';
p="$cdli$photo${px}.jpg"
res=`curl -o /dev/null -s -w '%{http_code}' $p`
if [ "$res" == "200" ]; then
    if [ "$use_eic" = "1" ]; then
	curl -o $ORACC/eic/$px.jpg -s $p
	echo "eic	eic/$px.jpg"
    else
	echo "cdli	dl/photo/${px}.jpg"
    fi
fi
l="$cdli$line${px}_l.jpg"
res=`curl -o /dev/null -s -w '%{http_code}' $l`
if [ "$res" == "200" ]; then
    if [ "$use_eic" = "1" ]; then
	curl -o $ORACC/eic/${px}_l.jpg -s $l
	echo "eic	eic/${px}_l.jpg"
    else
	echo "cdli	dl/lineart/${px}_l.jpg"
    fi
fi
