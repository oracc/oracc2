#!/bin/dash

echo $0 $*

project=`oraccopt`
aslauto=`oraccopt . asl-auto`
aslstats=`oraccopt . asl-stats`
aslinsts=`oraccopt . asl-insts`
asldomain=`oraccopt . asl-domain`

tok=01tmp/g.tok
tis=01tmp/g.tis

if [ "$aslauto" = "yes" ]; then
    if [ "$aslstats" != "no" ]; then
	aslstats="yes"
    fi
fi

if [ "$asldomain" = "" ]; then
    asldomain=sl
fi

echo "$0: asl-auto=$aslauto; asl-stats=$aslstats"

if [ "$aslstats" = "yes" ]; then
    echo "$0: generating $tok and $tis"
    mkdir -p 02pub/tok
    tokx <01bld/lists/xtfindex.lst >$tok
    tok2tis.sh <$tok >$tis
    tokx -s -c <01bld/sux/summaries.xml | cbdex >01tmp/cbd1.tok
    tokx -s -c <01bld/qpn/summaries.xml | cbdex >01tmp/cbd2.tok
    sort -u 01tmp/cbd[12].tok >01tmp/cbd.tok
fi

if [ "$aslauto" = "yes" ]; then
    # create signlist in 01tmp/corpus.asl and do the XML and index
    auto-asl.sh $tok
    mcu-slix.sh $project 01tmp/auto.asl
    asl=02xml/sl.xml
else
    set 00lib/*.asl
    if [ "$1" != "00lib/*.asl" ]; then
	if [ -r $tis ]; then
	    sx -I$asldomain:$tis -X 02xml/sl.xml $1
	fi
	asl=02xml/sl.xml
    fi
fi

#if [ "$asl" != "" ]; then
#    if [ "$aslstats" = "yes" ]; then
#	tisify 01tmp/g.tis < 02xml/sl.xml >02xml/sl-tis.xml
#	mv 02xml/sl-tis.xml 02xml/sl.xml
#    fi
# #    if [ "$aslinsts" != "no" ]; then
# #	mkdir -p 02www/inst
# #	tisdiv -o -h -d 02www/inst 01tmp/g.tis
# #	chmod -R o+r 02www/inst
# #    fi
#fi

# provides=$ORACC_BUILDS/$project/02xml/provides-instances.xml
# (cd $ORACC_BUILDS/www ; oid-files.sh $project/inst html >$provides)
# chmod o+r $provides

sxweb.sh

# odo-corpus-signlist is called after glossaries are built so it's
# time to run tisdbi
tisdbi.sh
