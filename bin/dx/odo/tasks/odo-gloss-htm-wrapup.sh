#!/bin/sh

echo $0 $*

# Do this late so that all the necessary support files have been built
buildlex=`oraccopt . build-lex-data`
if [ "$buildlex" = "yes" ]; then
    odo-lexdata.sh
fi

if [ "$l" = "sux" ]; then
    jumps=`oraccopt . outline-cbd-jumps`
    if [ "$jumps" == "yes" ]; then
	ns-art-outlines.sh
    fi
fi
