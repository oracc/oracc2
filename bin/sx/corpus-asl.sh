#!/bin/sh
#
# TODO:
#	unify auto-asl.sh and corpus-asl.sh--just use different lists
#	modify odo-corpus-signlist.sh to use unified corpus-asl.sh
# 	move all sl.xml work to mcu-slix.sh
#
# This script is analagous to auto-asl.sh but the sign list is based on a
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

project=`oraccopt`
aslauto=`oraccopt . asl-auto`
aslstats=`oraccopt . asl-stats`
aslinsts=`oraccopt . asl-insts`
asldomain=`oraccopt . asl-domain`

if [ "$aslauto" == "yes" ]; then
    aslstats="yes"
    aslinsts="yes"
fi

if [ "$asldomain" == "" ]; then
    asldomain="sl"
fi

tok=01tmp/g.tok
tis=01tmp/g.tis

project=$1
list=$2

if [ "$project" == "" ] || [ "$list" == "" ] ; then
    echo $0: must give project and list on command line. Stop.
    exit 1
fi

libscripts="${ORACC_BUILDS}/lib/scripts"
oproject="${ORACC_BUILDS}/$project"
mkdir -p 01tmp

if [ "$aslstats" = "yes" ]; then
    echo "$0: generating $tok and $tis"
    mkdir -p 02pub/tok
    tokx <01bld/lists/xtfindex.lst >$tok
    tok2tis.sh <$tok >$tis
    tokx -s -c <01bld/sux/summaries.xml | cbdex >01tmp/cbd1.tok
    tokx -s -c <01bld/qpn/summaries.xml | cbdex >01tmp/cbd2.tok
    sort -u 01tmp/cbd[12].tok >01tmp/cbd.tok
fi

merge=`oraccopt $project asl-merge`
if [ "$merge" != "" ]; then
    mopt="-R $merge"
fi

if [ -r "$oproject/00lib/corpus-asl.txt" ]; then
    h="-h $oproject/00lib/corpus-asl.txt"
fi

(cd 01tmp ; cut -f2 <csl.kis | tail -n +2 >csl.key ; sx -Ksl:csl.key $mopt >corpus.asl)

# Old approach: move to mcu-slix.sh
#(cd 01tmp ; sx -Isl:corpus.kis -x corpus.asl)
#
# Old approach: need to unify build outputs and move them post mcu-slix.sh
xsltproc $libscripts/sxweb-freq-tab.xsl 01tmp/sl.xml >01tmp/freq-tab.xml
mkdir -p 00etc
reptab=`basename $list .lst`-rep.tab
(cd 01tmp ; xsltproc ${libscripts}/sx-repertoire.xsl sl.xml >../00etc/$reptab)
ls -l 01tmp/sl.xml 00etc/$reptab
