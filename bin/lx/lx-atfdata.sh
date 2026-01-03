#!/bin/dash

#set -x

echo $0 $*

lxd=01bld/lists
project=`oraccopt`

if [ "$project" = "" ]; then
    >&2 echo $0: no project found. Stop.
    exit 1
fi

rm -f 02pub/atf-data.tab

set 00atf/*.atf
if [ "$1" != "00atf/*.atf" ]; then
    catmode=`oraccopt . catalog-build-policy`
    if [ "$catmode" != "" ]; then
	catarg="-c $catmode"
    fi
    cut -b 1- 00atf/*.atf | atfdatax $catarg -p $project >>02pub/atf-data.tab
elif [ -r 00lib/umbrella.lst ]; then
    lx -a 00lib/umbrella.lst -p $project >02pub/atf-data.tab
elif [ -r 00lib/search.lst ]; then
    lx -a 00lib/search.lst >02pub/atf-data.tab
elif [ -r 00lib/proxy.lst ]; then
    true
else
    >&2 echo $0: no atf files found in project $project. Stop.
    exit 1
fi

set $lxd/proxy*.lst
if [ "$1" != "$lxd/proxy*.lst" ]; then
    >&2 ls -l $lxd/proxy*.lst
    if [ -r $lxd/proxy.lst ] || [ -r $lxd/proxy-cat.lst ]; then
	>&2   echo $0: processing $lxd/proxy*.lst
	if [ -s 02pub/atf-data.tab ]; then
	    lx -ui 02pub/atf-data.tab +? $lxd/proxy.lst +? $lxd/proxy-cat.lst
	elif [ -s $lxd/proxy.lst ]; then
	    lx -o 02pub/atf-data.tab -u $lxd/proxy.lst +? $lxd/proxy-cat.lst
	elif [ -s $lxd/proxy-cat.lst ]; then
	    lx -o 02pub/atf-data.tab -u $lxd/proxy-cat.lst
	else
	    >&2 echo $0: project $project contains unknown proxy list in $*. Stop.
	    exit 1
	fi
    fi
fi

chmod o+r 02pub/atf-data.tab
