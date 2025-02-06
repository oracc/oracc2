#!/bin/sh
#
# $1 = PX number
# $2 = output directory for PX page building
# $3 = project (optional, only needed if caching is requested)
#
# Create an image div for the PX number $1 in file $2
#
px=$1
div=$2
project=
#$3

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
# /home/oracc/ as we go; this leaves names such as eic/P123456.jpg
if [ "$1" != "$try" ]; then
    found=1
    x=ooi
    for a in $try; do
	echo "ooi	$a"
    done | sed "s#$ORACC/##" >/tmp/$$.pxi
fi

# Now try the cache
if [ "$found" != "1" ]; then
    eic=$ORACC/eic
    try=$eic/$px'*'
    set $try
    if [ "$1" != "$try" ]; then
	found=1
	x=eic
	for a in $*; do
	    echo "eic	$a"
	done | sed "s#$ORACC/##" >/tmp/$$.pxi
    fi
fi

if [ "$found" != "1" ]; then
    # Now try the CDLI img db
    try=`$ORACC/bin/dbx -v -d ${ORACC}/cdli/02pub/img -n img $px | tr ' ' '\n' | grep -v 'tn_\|_d\|svg'`
    if [ "$try" != "" ]; then
	found=1
	x=dbx
	if [ ! "$project" = "" ] && [ -r $ORACC/$project/.eic ]; then
	    use_eic=1
	fi
	for a in $try; do
	    if [ "$use_eic" = "1" ]; then
		b=`basename $a`
		p="$cdli${a}"
		curl -o $eic/$b -s $p
		echo "eic	eic/$b"
	    else
		echo "cdli	$a"
	    fi
	done >/tmp/$$.pxi
    fi
fi

# Now try the XMD metadata for @src entries
if [ "$found" != "1" ]; then
    four="${px:0:4}"
    cxmd=$ORACC/cdli/01bld/$four/$px/$px.xmd
    if [ -s "$cxmd" ]; then
	xcdli=`xmllint -xpath './/*[@src]' $cxmd 2>/dev/null | grep -v 'tn_\|_d\|svg' | cut -d'"' -f6 | sort -t/ -k2`
    fi
    if [ "$xcdli" != "" ]; then
	found=1
	x=xmd
	for a in $xcdli; do
	    echo "cdli	dl/$a"
	done >>/tmp/$$.pxi
    fi
fi

if [ "$found" != "1" ]; then
    x=crl
    $ORACC/bin/ispimg-curl.sh $px $use_eic >/tmp/$$.pxi
fi

if [ -s /tmp/$$.pxi ]; then
    echo '<div id="p4Images"><h1 class="p3h2 border-top heading">Images</h1>' >$div
    while read typ loc
    do
	case $typ in
	    eic)
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
  <div class="caption">[$x] $origin image from <a href="$host$url">$url</a></div>
</div>
EOF
    done </tmp/$$.pxi
    echo '</div>'>>$div
fi
#rm -f /tmp/$$.pxi
