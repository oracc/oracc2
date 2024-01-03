#!/bin/dash
#
# mcu-lemm.sh checks the glossary files and updates the 02pub/*-lemm.sig
# lemmatization data.
#
# With a non-zero first argument the 02pub/lemm*.sig files are
# updated; otherwise only checking is performed.
#
# The functionality of l2p1.sh and o2-cbdpp.sh, and also their
# sometime caller o2-update-lem.sh, are rolled into this script and
# its makefile mcu-lemm.mk.
#
# The checking/updating done by mcu-lemm overlaps with
# o2-super-prx-sig.sh which is not yet accounted for except that the
# use of .nolemmdata has been dropped from mcu-lemm because that was a
# flag file touched by o2-super-prx-sig.sh as part of o2-super-prep.sh 
#

lemm=$1

projtype=`oraccopt . type`

rm -f .badgloss

if [ "$projtype" = "portal" ] || [ $projtype = "signlist" ]; then
    true
elif [ "$projtype" = "superglo" ]; then
    if [ "$lemm" != "" ]; then
	lemmsuper="lemmsuper"
    fi
    set 00src/*.glo
    if [ "$1" != "00src/*.glo" ]; then
	if [ "$2" != "" ]; then
	    echo "$0: superglo limited to one glossary (found $*). Stop."
	    exit 1
	else
	    make -f $ORACC_BUILDS/lib/data/mcu-lemm.mk super $lemmsuper
	fi
    else
	echo "$0: superglo has no glossary in 00src. Stop."
	exit 1
    fi
else
    if [ "$lemm" != "" ]; then
	lemmgloss="lemmgloss"
    fi
    set 00src/*.glo
    if [ "$1" != "00src/*.glo" ]; then
	echo "$0: ignoring $* because project is not a superglo"
    fi
    set 00lib/*.glo
    if [ "$1" != "00lib/*.glo" ]; then
	make -f $ORACC_BUILDS/lib/data/mcu-lemm.mk check $lemmgloss
    else
	echo $0: no glossaries found in 00lib.
    fi
fi

if [ -r .badgloss ]; then
    exit 1
else
    exit 0
fi
