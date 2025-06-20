#!/bin/dash
#OLD# $1=project $2=input $3=output $4=cemd $5=state $6=glos-lang
#NEW# $1=project $2=tsv $3=key $4=output $5=cemd $6=state $7=glos-lang $8=-O if ip->ood
>&2 echo px: called $0 $*
bin=${ORACC}/bin
lib=${ORACC}/lib
o="-stringparam oracc ${ORACC}"
p="-stringparam project $1"
if [ "$5" = "ccat" ]; then
    S=-S$6
    $bin/tx -s -t $2 -k $3 | $bin/cex $8 $S -p $1 -icat |
	xsltproc $o $p $lib/scripts/isp-ce-HTML.xsl - >$4
elif [ "$5" = "ctra" ]; then
    S=-S$6
    $bin/tx -s -t $2 -k $3 | $bin/cex $S -p $1 -itra |
	xsltproc $o $p $lib/scripts/isp-ce-HTML.xsl - >$4
elif [ "$5" = "cglo" ]; then
    if [ "$7" = "" ]; then
	>&2 echo $0: cemd=cglo but glos-lang is empty. Stop.
	exit 1
    fi
    >&2 echo $0 isp-ce-HTML params $o and $p
    cat $2 | $bin/cex -p $1 -icbd/$7 | 
	xsltproc -stringparam fragment yes $o $p $lib/scripts/isp-ce-HTML.xsl - >$4
else
    if [ "$5" = "unit" ]; then
	cemd=-u
    else
	cemd=-l
    fi
    $bin/tx -s -t $2 -k $3 | $bin/wmx -p $1 $cemd | $bin/cextfx -4 -p $1 $cemd | \
	xsltproc $o $p $lib/scripts/isp-ce-HTML.xsl - >$4
fi
# remove the output if it's empty
if [ ! -s $4 ]; then
    rm -f $4
    exit 1
fi

