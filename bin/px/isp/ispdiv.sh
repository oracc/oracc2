#!/bin/dash
#OLD# $1=project $2=input $3=output $4=cemd $5=state $6=glos-lang
#NEW# $1=project $2=tsv $3=key $4=output $5=cemd $6=state $7=glos-lang
>&2 echo $0 $*
bin=/home/oracc/bin
lib=/home/oracc/lib
if [ "$5" = "ccat" ]; then
    S=-S$6
    $bin/tx -s -t $2 -k $3 | $bin/cex $S -p $1 -icat |
	xsltproc -stringparam project $1 $lib/scripts/isp-ce-HTML.xsl - \
		 >$4
elif [ "$5" = "cglo" ]; then
    if [ "$7" = "" ]; then
	echo $0: cemd=cglo but glos-lang is empty. >&2
	exit 1
    fi
    cat $2 | $bin/cex -p $1 -icbd/$7 | 
	xsltproc -stringparam fragment yes -stringparam project $1 \
		 $lib/scripts/isp-ce-HTML.xsl - \
		 >$4
else
    $bin/tx -s -t $2 -k $3 | $bin/wmx -p $1 -l | $bin/cextfx -4 -p $1 -l | \
	xsltproc -stringparam project $1 $lib/scripts/isp-ce-HTML.xsl - \
		     >$4
fi
# remove the output if it's empty
if [ ! -s $4 ]; then
    rm -f $4
    exit 1
fi

