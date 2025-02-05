#!/bin/sh
#
# $1 = PX number
# $2 = output directory for PX page building
#
# Create an image div for the PX number $1 in directory $2
#
px=$1
img=$2

cdli='https://cdli.mpiwg-berlin.mpg.de';
photo='dl/photo/';
line='dl/lineart/';

if [ "$px" == "" ] || [ "$img" == "" ]; then
    echo $0: must give PX and IMG on commandline. Stop.
    exit 1
fi
if [[ ! "$px" =~ [PX][0-9]{6} ]]; then
    echo $0: malformed PX number. Stop.
    exit 1
fi
# just in case this exists ...
rm -f /tmp/$$.pxi
imgdir=`dirname $img`
mkdir -p $imgdir
if [ ! -d $imgdir ] || [ ! -w $imgdir ]; then
    echo $0: .img file directory $imgdir non-existent or not writeable. Stop.
    exit 1
fi

# First try the oracc online images (ooi)
#four="${px:0:4}"

ooi=$ORACC/ooi
try=$ooi/$px'*'
set $try
# if found echo the names of found items into the .pxi stripping
# /home/oracc/ as we go; this leaves names such as eic/P123456.jpg
if [ "$1" != "$try" ]; then
    found=1
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
	for a in $try; do
	    echo "eic	$a"
	done | sed "s#$ORACC/##" >/tmp/$$.pxi
    fi
fi

if [ "$found" != "1" ]; then
    # Now try the CDLI img db
    try=`dbx -v -d $ORACC/cdli/02pub/img -n img $px | tr ' ' '\n' | grep -v 'tn_\|_d'`
    if [ "$try" != "" ]; then
	found=1
	if [ ! "$project" = "" ] && [ -r $ORACC/$project/.eic ]; then
	    use_eic=1
	    #	mkdir -p $ORACC/www/p4.d/eic/$four
	fi
	for a in $try; do
	    if [ "$use_eic" = "1" ]; then
		b=`basename $a`
		p="$cdli${a}"
		curl -o $eic/$b -s $p
		echo "eic	$b"
	    else
		echo "cdli	$a"
	    fi
	done >/tmp/$$.pxi
    fi
fi

# Now try the XMD metadata for @src entries
if [ "$found" != "1" ]; then
    cxmd=$ORACC/cdli/01bld/$four/$px/$px.xmd
    if [ -s $cxmd ]; then
	xcdli=`xmllint -xpath './/*[@src]' $cxmd 2>/dev/null | grep -v tn_ | cut -d'"' -f6 | sort -t/ -k2`
    fi
    if [ "$xcdli" != "" ]; then
	found=1
	for a in $xcdli; do
	    echo "cdli	dl/$a"
	done >>/tmp/$$.pxi
    else
	# If we still didn't find anything probe CDLI for photo and/or lineart
	# echo $0: Performing slow CDLI check ...
	if [ ! "$project" = "" ] && [ -r $ORACC/$project/.eic ]; then
	    use_eic=1
	    mkdir -p $ORACC/www/p4.d/eic/$four
#	    echo $0: use_eic=$use_eic
	fi
	p="$cdli$photo${px}.jpg"
	res=`curl -o /dev/null -s -w '%{http_code}' $p`
	if [ "$res" == "200" ]; then
	    # echo $0: found $p
	    mkdir -p $eic
	    if [ "$use_eic" = "1" ]; then
		curl -o $eic/$px.jpg -s $p
		echo "eic	$px"
	    else
		echo "cdli	dl/photo/${px}.jpg" >>/tmp/$$.pxi
	    fi
	fi
	l="$cdli$line${px}_l.jpg"
	res=`curl -o /dev/null -s -w '%{http_code}' $l`
	if [ "$res" == "200" ]; then
	    # echo $0: found $l
	    mkdir -p $eic
	    if [ "$use_eic" = "1" ]; then
		curl -o $eic/${px}_l.jpg -s $p
		echo "eic	${px}_l"
	    else
		echo "cdli	dl/lineart/${px}.jpg" >>/tmp/$$.pxi
	    fi
	fi
    fi
fi

if [ -s /tmp/$$.pxi ]; then
    while read typ loc
    do
	case $typ in
	    eic)
		host=
		url="/$loc"
	    ;;
	    ooi)
		host=
		url="/$loc"
	    ;;
	    cdli)
		host=$cdli
		u="${cdli}/$loc"
	    ;;
	esac
    done </tmp/$$.pxi
fi
echo '<div id="p4Images"><h1 class="p3h2 border-top heading">Images</h1>' >$img
cat <<EOF >>$img
<div class="image"><img src="$host/$url" alt="image of $n"/>
  <div class="caption">$origin image from <a href="$host/$url">$u</a></div>
</div>
EOF
echo '</div>'>>$img

# rm -f /tmp/$$.pxi
