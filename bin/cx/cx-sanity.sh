#!/bin/sh
#
# CX has some requirements that are probably already met by all Oracc
# catalogues but this script checks the requirements and exits 1 if
# either is not met.
#
# This script must be run after cx-marshall.sh because it requires
# .tsv with ID in column 1.
#
ldir=01bld/cat/*.tsv
set $ldir
if [ "$1" != "${ldir}" ]; then
    # 1a) catalogues may not mix id_text and id_composite
    for c in $* ; do
	head -1 $c | grep -q id_text
	if [ $? -eq 0 ]; then
	    head -1 $c | grep -q id_composite
	    if [ $? -eq 0 ]; then
		echo $0: $c mixes id_text and id_composite. Stop.
		exit 1
	    fi
	fi
	# 1b) catalogues may not mix Q and X IDs
	if $(grep -q ^Q $c); then
	    if $(grep -q ^X $c); then
		echo $0: $c mixes Q and X IDs which is not allowed. Stop.
		exit 1
	    fi
	fi
    done
    # 2) catalogues may not have multiple files of the same type, i.e.,
    #    P/Q/X
    for pqx in P Q X ; do
	n=`grep -l ^${pqx} $* | wc -l`
	if [ $n -gt 1 ]; then
	    echo $0: multiple files with ${PQX} ids not allowed. Stop.
	    exit 1
	fi
    done	       
fi
