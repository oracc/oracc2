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
echo $0 $*

# Note that adding lem to the list here also does atf check
if [ "$3" = "" ]; then
    set sl glo lem
    echo $0 check list set to $*
fi

for a in $* ; do
    case $a in
	atf) odo-check-atf.sh ;;
	glo) odo-check-glo.sh ;;
	lem) odo-check-atf.sh -m ;;
	sl)  odo-check-asl.sh ;;
    esac
done
