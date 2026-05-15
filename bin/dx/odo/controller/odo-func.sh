#!/bin/sh
set -a

odo_odo ()
{
    if [ "$odo_odo" != "odo" ]; then
	b=`basename $1 .sh`
	c=`/bin/echo -n $b | cut -d- -f2`
	echo $1: direct use not allowed: say "'"oracc $c"'". Stop.
	exit 1
    else
	odov $*
    fi
}

odov ()
{
    if [ "$odo_verb" = "-v" ]; then
	echo $*
    fi
}
