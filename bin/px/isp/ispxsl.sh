#!/bin/dash
#
# This is a version of ispxtf.sh which handles application of a
# generic XSL script to generic IDs.
#
# $1=host-project $2=prox-project $3=item $4=tr-lang ("-" for no tr-lang)
# $5=meta-dir $6=html-dir $7=XSL-script $8=cachedir $9=type
#
# If this is not a proxy then prox-project is set to host-project so
# it's safe to use prox-project for all XTF-related paths.
#
# $3=item is must be obtainable via $9=type in a resource case
# statement below.
#
# Exit
#	0 on success
#	1 on error
#	2 type not known in resource case
#	3 item retrieval failed
#

>&2 echo $0 $*

oraccbin=${ORACC}/bin
host_project=$1
prox_project=$2
item=$3
if [ "$4" != "-" ]; then
    tr=$4
fi
metadir=$5
htmldir=$6
xslt=$7
cachedir=$8
type=$9

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

oraccarg="-param oracc '$ORACC'"
projarg="-param project '$prox_project'"

# Check we have or can make the metadir--this ends with
# p4/htm/P123/123456 so it prepares for HTML generation as well
>&2 mkdir -v -p $metadir || exit 1

# Check we have or can make the htmldir if it differs from metadir
# (which it will for proxies)
if [ "$metadir" != "$htmldir" ]; then
    >&2 mkdir -v -p $htmldir || exit 1
fi

# identify or generate the required source data
case $type in
    ood)
	input=$metadir/gdf.xml
	>&2 $oraccbin/ood-item.sh $prox_project $item $input
	if [ $? -ne 0 ]; then
	    echo $0: unknown $oraccbin/ood-item.sh $prox_project $item failed. Stop.
	    exit 3
	fi
    ;;
    *)
	echo $0: unknown type $type in resource case. Stop. >&2
	exit 2
    ;;
esac

# Check that we have the necessary source data
[ -r $input ] || exit 2

>&2 echo $0 xsltproc $oraccarg $projarg $xtlarg $xmdotl $xmd '>'$metadir/gdf.html
xsltproc $oraccarg $projarg $xslt $input >$metadir/gdf.html
if [ $? -ne 0 ]; then
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
