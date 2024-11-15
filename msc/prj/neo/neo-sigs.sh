#!/bin/sh
neosig=01tmp/neo.sig
rm -f $neosig
for a in `cat 00lib/order.lst`; do
    xsig=$ORACC/$a/01bld/from-xtf-glo.sig
    if [ -r $xsig ]; then
	grep -v ^@fields $xsig
    fi
done \
    | sigx -b -n >01bld/project.sig
