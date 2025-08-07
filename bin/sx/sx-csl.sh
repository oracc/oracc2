#!/bin/sh
echo $0 $*
cbdopt=`oraccopt . asl-suxword`
if [ "$cbdopt" = "yes" ]; then
    cbd=cbd
fi
make -f $ORACC/lib/data/Makefile.csl clean csl $cbd hook
