#!/bin/dash
#
# $1=host-project $2=prox-project $3=PQX $4=tr-lang ("-" for no tr-lang)
# $5=meta-dir $6=html-dir $7=xmdoutline.xsl $8=cachedir
#
# If this is not a proxy then prox-project is set to host-project so
# it's safe to use prox-project for all XTF-related paths.
#
# New oraccopt will support:
#
#   -p project -d default -a (search ancestor-or-self config) -s (search system config) OPTION
#
# xtlproject=`oraccopt -p $1 -d $1 -a catalog-master-project
#
# Exit
#	0 on success
#	1 on error
#	2 no XMD--text not in project
#	3 no XTF--transliteration not available
#

>&2 echo $0 $*

oraccbin=${ORACC}/bin
host_project=$1
prox_project=$2
PQX=$3
if [ "$4" != "-" ]; then
    tr=$4
fi
metadir=$5
htmldir=$6
xmdotl=$7
cachedir=$8

if [ "$cachedir" != "" ]; then
    errlog=$cachedir/err.log
    rm -f $errlog
    >&2 echo $0: args=$*
    echo $0: transferring error logging to $errlog >&2
    exec 2>$errlog
    echo $0 $* >&2
else
    exec 2>/dev/null
fi

xtl=`$oraccbin/pqxpand xtl $xtlproject:$PQX`
projarg="-param project '$prox_project'"
if [ -r $xtl ]; then
    xtlarg="-stringparam xtl $xtl"
fi

xmd=`$oraccbin/pqxpand xmd $prox_project:$PQX`
xtf=`$oraccbin/pqxpand xtf $prox_project:$PQX`

# Check that we have the necessary source data
[ -r $xmd ] || exit 2

# Check we have or can make the metadir--this ends with
# p4/htm/P123/P123456 so it prepares for HTML generation as well
mkdir -v -p $metadir || exit 1

# Check we have or can make the htmldir if it differs from metadir
# (which it will for proxies)
if [ "$metadir" != "$htmldir" ]; then
    mkdir -v -p $htmldir || exit 1
fi

xsltproc $projarg $xtlarg $xmdotl $xmd >$metadir/meta.xml
if [ $? -eq 0 ]; then
    [ -r $xtf ] || exit 3
    htxh=$metadir/$PQX.txh
    if [ ! -r $htxh ]; then
	txh=`$oraccbin/pqxpand txh $prox_project:$PQX`
	if [ -r $txh ]; then
	    >&2 echo $0: linking $txh to $htmldir
	    ln -sf $txh $htmldir
	else
	    >&2 echo $0: creating $htxh
	    $oraccbin/xtf2txh -i $prox_project:$PQX
	fi
    else
	>&2 echo $0: $htxh already exists
    fi
    if [ $? -eq 0 ]; then
	if [ "$tr" != "" ]; then
	    trarg="-t $tr"
	fi
	$oraccbin/xmd2htm $trarg -i $prox_project:$PQX
	if [ ! $? -eq 0 ]; then
	    echo "Failed call invoked as:" >&2
	    echo "	$oraccbin/xmd2htm -i $prox_project:$PQX" >&2
	    echo $0: xmd2htm failed. Stop. >&2
	    exit 1
	fi
    else
	echo "Failed call invoked as:" >&2
	echo "	$oraccbin/xtf2txh -i $prox_project:$PQX" >&2
	echo $0: xtf2txh failed. Stop. >&2
	exit 1
    fi
else
    echo "Failed call invoked as:" >&2
    echo "	xsltproc $projarg $xtlarg $xmdotl $xmd >$metadir/meta.xml" >&2
    echo $0: xsltproc failed. Stop. >&2
    exit 1
fi
#
# Removing the errlog when exiting 0 is the right thing to do but it
# means that we have to be careful in the above to test $? as we go
# and exit 1 appropriately.
#
##if [ $? -eq 0 ]; then
##    rm -f $errlog
##fi

# exit with last program's status; don't exit 0/1 based on empty
# errlog because that could contain diagnostics
