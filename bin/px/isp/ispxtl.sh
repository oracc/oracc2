#!/bin/dash
# $1=project $2=xtl $3=output
>&2 echo $0 $*
bin=${ORACC}/bin
lib=${ORACC}/lib
xsltproc -stringparam project $1 $lib/scripts/isp-xtl2lst.xsl $2 >$3
if [ ! -s $3 ]; then
    rm -f $3
    exit 1
fi

