#!/bin/dash

echo $0 $*

project=`oraccopt`
aslauto=`oraccopt . asl-auto`
aslstats=`oraccopt . asl-stats`
aslinsts=`oraccopt . asl-insts`

if [ "$aslauto" = "yes" ]; then
    if [ "$aslstats" != "no" ]; then
	aslstats="yes"
    fi
fi

if [ "$aslstats" = "yes" ]; then
    tokx < 01bld/lists/xtfindex.lst >01tmp/g.tok
    tok2tis <01tmp/g.tok >01tmp/g.tis
fi

if [ "$aslauto" = "yes" ]; then
    # create signlist in 01tmp/corpus.asl and do the XML and index
    auto-asl.sh 01tmp/g.tok
    mcu-slix.sh $project 01tmp/auto.asl
else
    set 00lib/*.asl
    if [ "$1" != "00lib/*.asl" ]; then
	asl=02xml/sl.xml
    fi
fi

if [ "$asl" != "" ]; then
    if [ "$aslstats" = "yes" ]; then
	tisify 01tmp/g.tis < 02xml/sl.xml >02xml/sl-tis.xml
	mv 02xml/sl-tis.xml 02xml/sl.xml
    fi
    if [ "$aslinsts" != "no" ]; then
	tisdiv -h -d 02www/inst 01tmp/g.tis
	chmod -R o+r 02www/inst
	provides=$ORACC_BUILDS/$project/02xml/provides-instances.xml
	(cd $ORACC_BUILDS/www ; oid-files.sh $project/inst html >$provides)
	chmod o+r $provides
    fi
fi

sxweb.sh

# odo-corpus-signlist is called after glossaries are built so it's
# time to run tisdbi
tisdbi.sh
