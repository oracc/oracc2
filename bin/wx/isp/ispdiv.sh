#!/bin/dash
# $1=project $2=input $3=output
wm -p $1 -l -i $2 | ce_xtf -4 -p $1 -l | \
    xsltproc -stringparam project $1 ${ORACC_BUILDS}/lib/scripts/isp-ce-HTML.xsl - >$3
