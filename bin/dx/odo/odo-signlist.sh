#!/bin/dash
#
# Perform build for a signlist project.
#
# odo always does an odo-update which calls sx-slix.sh so there's no
# need to create the indices here unless this is working within the
# old o2 system.
#
echo $0 $*
o2=$1
projtype=`oraccopt . type`
asl=""

# This block only for corpus projects which auto-create their sign list
if [ "$projtype" = "corpus" ]; then
    aslauto=`oraccopt . asl-auto`
    if [ "$aslauto" = "yes" ]; then
	sx-csl.sh $tok
	sx-slix.sh $project 01tmp/csl.asl
	asl="01tmp/csl.asl"
	o2=
    fi
fi

# This block for any project that has a static sign list
if [ "$asl" = "" ]; then
    set 00lib/*.asl
    if [ "$1" = "00lib/*.asl" ]; then
	false
    else
	asl=$1
    fi
    
    aslstats=`oraccopt . asl-stats`
    if [ "$aslstats" = "yes" ]; then
	sx-csl.sh $tok
    fi
    
    if [ "$o2" = "o2" ]; then
	sx-slix.sh `oraccopt` $asl
    fi
fi
if [ "$asl" != "" ]; then
    sx-bib.sh
    sxweb.sh
#    tisdbi.sh # this is not used at the moment but remember it in a possible ePSD3 tok implementation
    if [ "$projtype" = "signlist" ]; then
	odo-portal.sh
    fi
fi
