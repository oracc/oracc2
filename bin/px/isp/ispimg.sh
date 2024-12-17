#!/bin/sh
#
# $1 = PX number
# $2 = output directory
# $3 = cache
#
# Create an image div for the PX number $1 in directory $2
#
px=$1
img=$2
cache=$3
if [ "$px" == "" ] || [ "$img" == "" ]; then
    echo $0: must give PX and IMG on commandline. Stop.
    exit 1
fi
if [[ ! "$px" =~ [PX][0-9]{6} ]]; then
    echo $0: malformed PX number. Stop.
    exit 1
fi
imgdir=`dirname $img`
mkdir -p $imgdir
if [ ! -d $imgdir ] || [ ! -w $imgdir ]; then
    echo $0: .img file directory $imgdir non-existent or not writeable. Stop.
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
	cdli=`xmllint -xpath './/*[@src]' $cxmd 2>/dev/null | grep -v tn_ | cut -d'"' -f6 | sort -t/ -k2`
    fi
    if [ "$cdli" != "" ]; then
	for a in $cdli; do
	    echo "cdli	dl/$a"
	done >>/tmp/$$.pxi
    else
	# echo $0: Performing slow CDLI check ...
	if [ ! "$project" = "" ] && [ -r $ORACC/$project/.eic ]; then
	    use_eic=1
	    mkdir -p $ORACC/www/p4.d/eic/$four
#	    echo $0: use_eic=$use_eic
	fi
	cdliurl='https://cdli.mpiwg-berlin.mpg.de/';
	photo='dl/photo/';
	line='dl/lineart/';
	p="$cdliurl$photo${px}.jpg"
	res=`curl -o /dev/null -s -w '%{http_code}' $p`
	if [ "$res" == "200" ]; then
	    # echo $0: found $p
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
	    # echo $0: found $l
	    mkdir -p $eic
	    if [ "$use_eic" = "1" ]; then
		curl -o $eic/${px}_l.jpg -s $p
	    else
		echo "cdli	dl/lineart/$px.pxi" >>/tmp/$$.pxi
	    fi
	fi
    fi
fi

echo '<div id="p4Images"><h1 class="p3h2 border-top heading">Images</h1>' >$img
if [ -s /tmp/$$.pxi ]; then
    while read typ url
    do
	case $typ in
	    eic)
		n=`basename $url .jpg`
		nn=`basename $n _l`
		u="https://cdli.mpiwg-berlin.mpg.de/$nn"
		cat <<EOF >>$img
<div class="image"><img src="/ooi/$n" alt="image of $n"/>
  <div class="caption">CDLI image from <a href="$u">$u</a></div>
</div>
EOF
	    ;;
	    ooi)
		n=`basename $url .png`
		cat <<EOF >>$img
<div class="image"><img src="$u" alt="image of $n"/>
  <div class="caption">Oracc image from <a href="$u">$u</a>.</div>
</div>
EOF
	    ;;
	    cdli)
		n=`basename $url .jpg`		
		u="https://cdli.mpiwg-berlin.mpg.de/$url"
		cat <<EOF >>$img
<div class="image"><img src="$u" alt="image of $n"/>
  <div class="caption">CDLI image from <a href="$u">$u</a></div>
</div>
EOF
	    ;;
	esac
    done </tmp/$$.pxi
fi
echo '</div>'>>$img

# rm -f /tmp/$$.pxi
