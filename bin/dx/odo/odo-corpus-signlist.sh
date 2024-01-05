#!/bin/dash

echo $0 $*

project=`oraccopt`
aslauto=`oraccopt . asl-auto`
aslstats=`oraccopt . asl-stats`
aslinsts=`oraccopt . asl-insts`

if [ "$aslauto" = "yes" ] || [ "$aslstats" = "yes" ] || [ "$aslinsts" = "yes" ]; then
    tok-g < 01bld/lists/xtfindex.lst >01tmp/g.tok
    tok2tis <01tmp/g.tok >01tmp/g.tis
fi

if [ "$aslauto" ]; then
    # create signlist in 01tmp/corpus.asl and do the XML and index
    # mcu-slix.sh $project 01tmp/corpus.asl
    asl=02xml/sl.xml
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
    if [ "$aslinsts" = "yes" ]; then
	rm -fr 01tmp/slinst
	mkdir -p 01tmp/slinst
	tisdiv -h -d 01tmp/slinst 01tmp/g.tis
    fi
fi

sxweb.sh
