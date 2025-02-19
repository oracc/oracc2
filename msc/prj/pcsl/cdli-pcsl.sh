#!/bin/sh
date=20241011
src=cdli-$date.tsv
cat=$date.tsv
atf=cdli-$date.atf
# Fix cdli cat src format to use P-id in first field
./id_text.sh <$src >$cat
# Get all Uruk IV/III
grep 'Uruk \(IV\|III\) (ca' $cat >pcsl-01-$cat
# Rem seals and composites
grep -v '(physical)\|sealing\|CDLI Seal' pcsl-01-$cat | grep -v 'composite"' >pcsl-02-$cat
# p-num list of pcsl-cat sofar
cut -f1 pcsl-02-$cat >pcsl-cat.lst
# Get texts with lang=qpc (excluding Elamite)
./qpc.plx <cdli-20241011.atf | sort -u >cdli-atf-qpc.lst
# List of texts not catalogued as Uruk IV/III but with lang=qpc in ATF
comm -23 cdli-atf-qpc.lst pcsl-cat.lst >atf-qpc-only.lst
# Union of cat list and atf-qpc list
sort -u pcsl-cat.lst cdli-atf-qpc.lst >pcsl-all.lst
# Get union entries from cat src
sed 's/$/	/' pcsl-all.lst | grep -f - $cat >pcsl-03-$cat
# Build Oracc format cat tsv starting with fields line from cat src
head -1 $cat | sed 's/artifact_id/id_text/' >pcsl.tsv
csvformat -tT <pcsl-03-$cat >>pcsl.tsv
# Get ATF corpus of all texts that ended up in pcsl cat
cut -f1 pcsl.tsv | atfsplit.plx -cat -list - cdli-$date.atf >pcsl.atf
# Package the core data
zip pcsl pcsl.tsv pcsl.atf
