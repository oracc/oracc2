#!/bin/sh
#
# $1 = PX number
# $2 = output directory for PX page building
# $3 = project (optional, only needed if caching is requested)
# $4 = 1 if caching is in use; empty or non-1 otherwise
#
# Create an image div for the PX number $1 in file $2
#
px=$1
div=$2
project=$3
use_oic=$4

cdli='https://cdli.mpiwg-berlin.mpg.de/';

if [ "$px" == "" ] || [ "$div" == "" ]; then
    echo $0: must give px-number and div-file on command line. Stop.
    exit 1
fi
if [[ ! "$px" =~ [PX][0-9]{6} ]]; then
    echo $0: malformed PX number. Stop.
    exit 1
fi
if [ -d $div ]; then
    echo $0: $div is not a regular file. Stop.
    exit 1
fi

# just in case this exists ...
pxi=/
rm -f /tmp/$$.pxi $div

divdir=`dirname $div`
mkdir -p $divdir
if [ ! -d $divdir ] || [ ! -w $divdir ]; then
    echo $0: $div file directory $divdir non-existent or not writeable. Stop.
    exit 1
fi

# First try the oracc online images (ooi)
ooi=$ORACC/ooi
try=$ooi/$px'*'
set $try
# if found echo the names of found items into the .pxi stripping
# /home/oracc/ as we go; this leaves names such as oic/P123456.jpg
if [ "$1" != "$try" ]; then
    found=1
    x=ooi
    for a in $try; do
	echo "ooi	$a"
    done | sed "s#$ORACC/##" >/tmp/$$.pxi
fi

# Now try the cache
if [ "$found" != "1" ]; then
    oic=$ORACC/oic
    try=$oic/$px'*'
    set $try
    if [ "$1" != "$try" ]; then
	found=1
	x=oic
	for a in $*; do
	    echo "oic	$a"
	done | sed "s#$ORACC/##" >/tmp/$$.pxi
    fi
fi

if [ "$found" != "1" ]; then
    # Now try the CDLI img db
    try=`$ORACC/bin/dbx -v -d ${ORACC}/cdli/02pub/img -n img $px | tr ' ' '\n' | grep -v 'tn_\|_d\|svg'`
    if [ "$try" != "" ]; then
	found=1
	x=dbx
	for a in $try; do
	    if [ "$use_oic" = "1" ]; then
		x=dbxc
		b=`basename $a`
		p="$cdli${a}"
		curl -o $oic/$b -s $p
		echo "oic	oic/$b"
	    else
		echo "cdli	$a"
	    fi
	done >/tmp/$$.pxi
    fi
fi

if [ -s /tmp/$$.pxi ]; then
    echo '<div id="p4Images"><h1 class="p3h2 border-top heading">Images</h1>' >$div
    while read typ loc
    do
	case $typ in
	    oic)
		host=
		url="/$loc"
		n=`basename $loc`
		origin="Cached CDLI"
	    ;;
	    ooi)
		host=
		url="/$loc"
		n=`basename $loc`
		origin="Oracc"
	    ;;
	    cdli)
		host=$cdli
		url=$loc
		n=`basename $loc`
		origin="CDLI"
	    ;;
	esac
	cat <<EOF >>$div
<div class="image"><img src="$host$url" alt="image of $n"/>
  <div class="caption">[$x] $origin image from <a target="_blank" href="$host$url">$url</a></div>
</div>
EOF
    done </tmp/$$.pxi
    echo '</div>'>>$div
    #rm -f /tmp/$$.pxi
    exit 0
else
    exit 2
fi
