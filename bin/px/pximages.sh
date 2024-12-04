#!/bin/sh
#
# $1 = PX number
# $2 = output directory
#
# Create an image div for the PX number $1 in directory $2
#
px=$1
outd=$2
project=$3
if [ "$px" == "" ] || [ "$outd" == "" ]; then
    echo $0: must give PX and OUTDIR on commandline. Stop.
    exit 1
fi
if [[ ! "$px" =~ [PX][0-9]{6} ]]; then
    echo $0: malformed PX number. Stop.
    exit 1
fi
if [ ! -d $outd ] || [ ! -w $outd ]; then
    echo $0: output directory non-existent or not writeable. Stop.
    exit 1
fi
four=`/bin/echo -n $px | sed 's/^\(....\).*$/\1/'`
ooi=$ORACC/ooi/$four
try=$ooi/$px'*'
set $try
if [ "$1" != "$try" ]; then
    found=1
    for a in $try; do
	echo "ooi	$a"
    done | sed "s#$ORACC/##" >/tmp/$$.pxi
fi
if [ "$found" != "1" ]; then
    eic=$ORACC/eic/$four
    try=$eic/$px'*'
#    echo $try
    set $try
    if [ "$1" != "$try" ]; then
	found=1
	for a in $try; do
	    echo "eic	$a"
	done | sed "s#$ORACC/##" >>/tmp/$$.pxi
    fi
fi
if [ "$found" != "1" ]; then
    cxmd=$ORACC/cdli/01bld/$four/$px/$px.xmd
    if [ -s $cxmd ]; then
	cdli=`xmllint -xpath './/*[@src]' $cxmd 2>/dev/null | grep -v tn_ | cut -d'"' -f6`
    fi
    if [ "$cdli" != "" ]; then
	for a in $cdli; do
	    echo "cdli	dl/$a"
	done >>/tmp/$$.pxi
    else
	echo $0: Performing slow CDLI check ...
	if [ ! "$project" = "" ] && [ -r $ORACC/$project/.eic ]; then
	    use_eic=1
#	    echo $0: use_eic=$use_eic
	fi
	cdliurl='https://cdli.mpiwg-berlin.mpg.de/';
	photo='dl/photo/';
	line='dl/lineart/';
	p="$cdliurl$photo${px}.jpg"
	res=`curl -o /dev/null -s -w '%{http_code}' $p`
	if [ "$res" == "200" ]; then
	    echo $0: found $p
	    mkdir -p $eic
	    if [ "$use_eic" = "1" ]; then
		curl -o $eic/$px.jpg -s $p
	    else
		echo "cdli	dl/photo/$px.pxi" >>/tmp/$$.pxi
	    fi
	fi
	l="$cdliurl$line${px}_l.jpg"
	res=`curl -o /dev/null -s -w '%{http_code}' $l`
	if [ "$res" == "200" ]; then
	    echo $0: found $l
	    mkdir -p $eic
	    if [ "$use_eic" = "1" ]; then
		curl -o $eic/${px}_l.jpg -s $p
	    else
		echo "cdli	dl/lineart/$px.pxi" >>/tmp/$$.pxi
	    fi
	fi
    fi
fi
#echo /tmp/$$.pxi:
if [ -s /tmp/$$.pxi ]; then
    cat /tmp/$$.pxi
fi
