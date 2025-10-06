#!/bin/sh
infile=$1
shift
if [ -r $infile ]; then
    s=`ls -l $infile | cut -d' ' -f5`
    CONTENT_TYPE="application/x-www-form-urlencoded" CONTENT_LENGTH=$s \
		./wx $* <$infile
else
    echo $0: no file given or $infile not readable. Stop.
fi
