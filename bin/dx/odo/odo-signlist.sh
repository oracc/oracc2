#!/bin/dash
#
# Perform build for a signlist project.
#
# odo always does an odo-update which calls sx-slix.sh so there's no
# need to create the indices here if unless this is working within the
# old o2 system.
#
echo $0 $*
o2=$1
projtype=`oraccopt . type`
asl=""
if [ "$projtype" = "corpus" ]; then
    aslauto=`oraccopt . asl-auto`
    if [ "$aslauto" = "yes" ]; then
	sx-csl.sh $tok
	sx-slix.sh $project 01tmp/csl.asl
	asl="01tmp/csl.asl"
    fi
else
    set 00lib/*.asl
    if [ "$1" = "00lib/*.asl" ]; then
	false
    else
	asl=$1
    fi
    if [ "$o2" = "o2" ]; then
	sx-slix.sh `oraccopt` $asl
    fi
    # TODO: locate stats for osl/pcsl
fi
if [ "$asl" != "" ]; then
    sxweb.sh
#    tisdbi.sh # this is not used at the moment but remember it in a possible ePSD3 tok implementation
    if [ "$projtype" = "signlist" ]; then
	odo-portal.sh
    fi
fi
