#!/bin/sh
pdfdir=/home/oracc/$1/01tmp
pdf=`/home/oracc/bin/atf2pdf.sh $1 $2 $pdfdir`
if [ -r $pdf ]; then
    echo $0 produced PDF $pdf
else
    >&2 echo $0 failed to produce PDF
fi
