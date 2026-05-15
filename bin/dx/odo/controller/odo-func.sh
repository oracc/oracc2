#!/bin/sh
set -a

odo_fail ()
{
    echo $*
    odo_quit
}

odo_log ()
{
    exec 1>&3
    exec 2>&4
    cat $odo_log
}

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

odo_quit ()
{
    echo $0: quitting because of previous error.
    odo_log
    odo_status error
    exit 1
}

odo_status ()
{
    if [ "$odo_tmpd" != "" ]; then
	if [ -d $odo_tmpd ]; then
	    odov $0 writing status $1 to $odo_tmpd/status
	    printf "$1" >$odo_tmpd/status;
	fi
    fi
}

odo_term ()
{
    odo_log
    odo_status done
    exit $?
}

odo_verbose ()
{
    if [ "$odo_verb" = "" ]; then
	if [ "$2" = "-v" ]; then
       	    export odo_verb=-v
	fi
	odov $1 verbose set to $odo_verb
    fi
}

odov ()
{
    if [ "$odo_verb" = "-v" ]; then
	echo $*
    fi
}
