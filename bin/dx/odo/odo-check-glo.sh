#!/bin/dash

echo $0 $*
rm -f 01log/glo.err
projtype=`oraccopt . type`

if [ "$projtype" = "signlist" ] || [ "$projtype" = "portal" ]; then
    exit 0
fi

if [ "$projtype" = "superglo" ]; then
    set 00src/*.glo
    if [ "$1" != "00src/*.glo" ]; then
	if [ "$2" != "" ]; then
	    echo "$0: superglo limited to one glossary (found $*). Stop."
	    exit 1
	else
	    exec 3>&1 4>&2 1>$log 2>&1
	    cbdpp.plx -c $1
	    exec 1>&3 2>&4
	    if [ -s $log ]; then
		echo $log >>01log/glo.err
	    fi
	fi
    else
	echo "$0: superglo has no glossary. Stop."
	exit 1
    fi
fi

set 00lib/*.glo
if [ "$1" != "00lib/*.glo" ]; then
    for a in $* ; do
	globase=`basename $a`
	if [ ! -r "00src/$globase" ]; then
	    log=01log/`basename $globase .glo`.log
	    echo $0 checking glossary $a
	    exec 3>&1 4>&2 1>$log 2>&1
	    cbdpp.plx -nosigs $a
	    exec 1>&3 2>&4
	    if [ -s $log ]; then
		echo $log >>01log/glo.err
	    fi
	fi
    done
fi

if [ -s 01log/glo.err ]; then
    for a in `cat 01log/glo.err` ; do
	cat $a
    done
    exit 1
else
    exit 0
fi
