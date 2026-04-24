#!/bin/sh
#
# Create a single catalogue which is the union of the argument catalogues.
#
# For each field, data from a later catalogue overwrites data from an earlier catalogue.
#
# The first field is always id_text; cx automatically coerces this to
# id_composite when generating XMD for a Q-ID
echo $0 $*
bin=${ORACC}/bin
tsv=$*
uf=01tmp/union.f
rm -f $uf
for t in $tsv ; do
    head -1 $t | tr '\t' '\n' | grep -v -x '^id_text\|id_composite' >>$uf
done
sort -u -o $uf $uf
ui=01tmp/union.i
rm -f $ui
for t in $tsv ; do
    grep -v '^id_\(text\|composite\)	' $t | cut -f1 >>$ui
done
sort -u -o $ui $ui
ut=01tmp/00cat/unionbase.tsv
echo id_text >idtext
cat idtext $uf | tr '\n' '\t' | sed 's/	$/\
/' >$ut
cat $ui >>$ut

$bin/rocox -U $ut $* >01bld/cat/union.tsv
