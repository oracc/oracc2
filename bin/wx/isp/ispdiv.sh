#!/bin/dash
# $1=project $2=input $3=output
wmx -p $1 -l -i $2 | cextfx -4 -p $1 -l | \
    xsltproc -stringparam project $1 ${ORACC_BUILDS}/lib/scripts/isp-ce-HTML.xsl - >$3
