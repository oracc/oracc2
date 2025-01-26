#!/bin/sh
if [ -r $1 ]; then
    nx -x -d $1
    xsltproc ${ORACC}/lib/scripts/numsys-sl.xsl numsys.xml >numsys-sl.xml
    xsltproc ${ORACC}/lib/scripts/numsys-html.xsl  numsys-sl.xml >numsys.html
else
    echo $0: must give nx data file to convert to html on command line. Stop.
    exit 1
fi
