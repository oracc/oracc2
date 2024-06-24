#!/bin/sh
proj=$1
item=$2
htmd=$3
pdfdir=$htmd
>&2 echo webpdf.sh building $1 $2 in $pdfdir
pdf=`/home/oracc/bin/atf2pdf.sh $1 $2 $pdfdir`
>&2 echo webpdf.sh got PDF file name $pdf
if [ -r $pdf ]; then
    pdfpath=`dirname $pdf`
    pdfbase=`basename $pdf`
    if [ "$pdfpath" != "$pdfdir" ]; then
	mv $pdf $pdfdir 2>/dev/null
	rm -fr `dirname $pdf`
    fi
    cd $pdfdir
    cat <<EOF
Content-type: application/pdf
Content-disposition: attachment; filename=$pdfbase

EOF
    exec cat $pdfbase
else
    >&2 echo $0 failed to produce PDF
fi
