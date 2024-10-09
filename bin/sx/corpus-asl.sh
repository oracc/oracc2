#!/bin/sh
#
# This script is analagous to auto.asl but the sign list is based on a
# list of sources rather than a project.  The outputs are 01tmp/sl.xml
# and a repertoire table, 01tmp/rep.tab.
#
# First argument is a project to use as a base for config info; second
# argument is a qualified PQX list so it can contain materials from
# multiple projects.
#
# The current directory must be writeable because the script uses
# 01tmp to do its work, creating the directory if necessary.
#
project=$1
list=$2

if [ "$project" == "" ] || [ "$list" == "" ] ; then
    echo $0: must give project and list on command line. Stop.
    exit 1
fi
libscripts="${ORACC_BUILDS}/lib/scripts"
oproject="${ORACC_BUILDS}/$project"
mkdir -p 01tmp

merge=`oraccopt $project asl-merge`
if [ "$merge" != "" ]; then
    mopt="-m $merge"
fi

if [ -r "$oproject/00lib/auto-asl.txt" ]; then
    h="-h $oproject/00lib/auto-asl.txt"
fi
gtoks=01tmp/corpus.tok
tokx <$list >$gtoks
# Old approach: deprecated.
#tok2asl $h -P $project $mopt -S auto $gtoks >01tmp/corpus.asl
# New approach: uses .kis suffix because .tis files have different data format
cat $gtoks | tokex -d01tmp | tokix -d01tmp -p$project | tokpct >01tmp/corpus.kis
# Old approach: deprecated.
#sx -Up osl -e -c 01tmp/corpus.asl >>01tmp/corpus.asl
# New approach: subsetting of osl is built in to sx via -K01tmp/corpus.key
(cd 01tmp ; cut -f2 <corpus.kis | tail -n +2 >corpus.key ; sx -Ksl:corpus.key >corpus.asl)
# Old approach: still used--subset now gets augmented with instance data
(cd 01tmp ; sx -Isl:corpus.kis -x corpus.asl)
xsltproc $libscripts/sxweb-freq-tab.xsl 01tmp/sl.xml >01tmp/freq-tab.xml
mkdir -p 00etc
reptab=`basename $list .lst`-rep.tab
(cd 01tmp ; xsltproc ${libscripts}/sx-repertoire.xsl sl.xml >../00etc/$reptab)
ls -l 01tmp/sl.xml 00etc/$reptab
