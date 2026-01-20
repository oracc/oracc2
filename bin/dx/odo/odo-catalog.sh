#!/bin/dash
echo $0 $*
# o2-lst.sh ## should be fully replaced by lx-lists.sh and cx(-marshall).sh
lx-lists.sh
atflinks.plx >01bld/atflinks.tab
linknorm.plx 01bld/atflinks.tab >01bld/normlinks.tab
## translist.sh >01bld/translist.txt ## replaced by atf-data.tab
xmd-sessions.plx
## xmdmanager2.plx ## replaced by cx.sh
cx.sh
xsltproc ${ORACC}/lib/scripts/xmd-summaries.xsl 01bld/cdlicat.xmd >01bld/cat-sum.xml
xsltproc ${ORACC}/lib/scripts/xmd-HTML-summaries.xsl 01bld/cat-sum.xml >01bld/cat-sum.html
xmd-check.plx
if [ -r 01bld/cdlicat.xmd ]; then
    project=`oraccopt`
    mkdir -p 02pub/cat
    echo qcatx $project 01bld/cdlicat.xmd
    qcatx -p $project -s <01bld/cdlicat.xmd
    sudo chmod -R go+r 02pub/cat
fi
