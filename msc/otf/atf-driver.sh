#!/bin/sh
project=$1
pqx=$2
atffile=$3
workdir=$4

if [ "$project" == "" ] || [ "$pqx" == "" ] || [ "$atffile" == "" ] || [ "$workdir" == "" ]; then
    echo $0: please give PROJECT PQX-ID PQX-PATH PDFDIR on commandline
    exit 1
fi

otfname="$pqx.otf"
pdfname="$pqx.pdf"
pdfwork="$workdir/$pdfname"

>&2 echo $0: pdf name $pdfwork

if [ -r $pdfwork ]; then
    >&2 echo $0: returning existing pdf $pdfwork
    /bin/echo -n $pdfwork
    exit 0
fi

xmd=`dirname $atffile`/$pqx.xmd
xmdo=/home/oracc/www/$project/xmdoutline.xsl
if [ ! -r $xmdo ]; then
    parent=`/bin/echo -n $project | cut -d/ -f1`
    xmdo=/home/oracc/www/$parent/xmdoutline.xsl
    if [ ! -r $xmdo ]; then
	xmdo=/home/oracc/lib/scripts/p3-xmd-div.xsl
    fi
fi

lastedit=`isogmt -f $atffile`

buildstamp=/home/oracc/bld/$project/buildstamp
servestamp=/home/oracc/$project/servestamp

if [ -r $servestamp ]; then
    lastx=served
    lastt=`isogmt -f $servestamp`
elif [ -r $buildstamp ]; then
    lastx=built
    lastt=`isogmt -f $buildstamp`
fi

bldatf=`/bin/echo -n $atffile | sed 's#^.*/bld/#[oracc]/bld/#'`

(cd $workdir;
 cat >$otfname <<EOF
@document

@project $project

@h1 Oracc Edition of $project : $pqx

@section introductionb

@h2 METADATA

EOF

xsltproc --stringparam project $project $xmdo $xmd | \
    xsltproc /home/oracc/lib/scripts/web-pdf-xmd.xsl - >>$otfname

cat >>$otfname <<EOF

@p @b{Credits:} `echo $project:$pqx | xmdfields.plx -b -f credits`

@p @b{Generator:} Oracc atf2pdf.sh at `isogmt -t`

@p @b{Production:} last $lastx at $lastt

@end section

@section edition

@h2 TEXT

@atffile $atffile

@end section

@end document
EOF

pdf-driver.sh $project $pqx $pdfname
)
/bin/echo -n $pdfwork
