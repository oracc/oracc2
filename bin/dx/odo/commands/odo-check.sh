#!/bin/sh
#
# odo-check.sh manages all the odo check XXX commands.
#
# The current options are:
#
# odo check
# odo check atf
# odo check lem
# odo check glo
# odo check sl
#
# args should be $1=user $2=project $3..$n=[check opts]
. ${ORACC}/bin/odo-func.sh
odo_odo $0 $*

shopt -s nullglob

# For atf the option good|bad or ok|no restricts the checking to one
# of the good/bad lists which are named ax.ok and ax.no

# Note that adding lem to the list here also does atf check
if [ "$1" = "" ]; then
    set sl glo lem
    odov $0 check list set to $*
fi

#
# oracc check does all of the checks set above
#
# oracc check (atf|atfv|lem|lemv) | glo | sl
#
# For atf group, additional args may be given which may be:
#
#   ok|no|good|bad | LIST_NAME | ATF_FILES
#
# TODO: check dups links
#

if [ "$odo_setx" = "yes" ]; then
    set -x
fi

chk=$*
for a in $* ; do
    case $a in
	atf)
	    shift
	    odo-atf.sh check $*
	    ;;
	atfv)
	    shift
	    odo-atf.sh check val $*
	    ;;
	glo)
	    shift
	    mcu-lemm.sh
	    if [ -r .badgloss ]; then
		odov $0: errors checking glossaries.
	    else
		odov $0: all glossaries check OK.
	    fi
	    ;;
	lem)
	    shift
	    odo-atf.sh check lem $*
	    ;;
	lemv)
	    shift
	    odo-atf.sh check lem val $*
	    ;;
	sl)
	    shift
	    asl=`ls 00lib/*.asl`
	    if [ "$1" != "00lib/*.asl" ]; then
		odov $0 checking signlist $asl
		sx -c $asl
	    fi
	    ;;
	ok|no|good|bad)
	    # ignore because these are passed to odo-atf.sh
	    ;;
	*)
	    odo_fail $0 unknown check argument $a
	    ;;
    esac
done
