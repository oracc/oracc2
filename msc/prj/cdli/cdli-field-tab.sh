#!/bin/sh
#
# Make a table of id_text and arg-field
cat=00cat/cdli_catalogue.tsv
field=$1
if [ "$field" = "" ]; then
    echo $0: must give exact field name on command line. Stop.
    exit 1
fi
if [ ! -r $cat ]; then
    echo $0: no $cat. Stop
    exit 1
fi
fnum=`head -1 00cat/cdli_catalogue.tsv | tr '\t' '\n' | grep -n -x "$field" | cut -d: -f1`
if [ "$fnum" = "" ]; then
    echo $0: field '$field' not found in $cat. Stop.
    exit 1
fi
mkdir -p 00etc
fhyph=`/bin/echo -n $field | tr ' ' -`
out=00etc/$fhyph.tsv
echo $0: making table of field '$field' with fnum=$fnum in $out
tail -n +2 $cat | cut -f1,$fnum >$out
