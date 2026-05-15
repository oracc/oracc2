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

# For atf the option good|bad or ok|no restricts the checking to one
# of the good/bad lists which are named ax.ok and ax.no

# Note that adding lem to the list here also does atf check
if [ "$3" = "" ]; then
    set sl glo lem
    echo $0 check list set to $*
fi

#
# TODO: check dups links
#
for a in $* ; do
    case $a in
	atf)
	    shift
	    odo-atf.sh $user $proj check $*
	    ;;
	glo)
	    shift
	    odo-check-glo.sh $*
	    ;;
	l|lem)
	    shift
	    odo-atf.sh $user $proj check lem $*
	    ;;
	sl)
	    shift
	    odo-check-asl.sh $*
	    ;;
	v|val)
	    shift
	    odo-atf.sh $user $proj check val $*
	    ;;	
	vl)
	    shift
	    odo-atf.sh $user $proj check val lem $*
	    ;;	
    esac
done
