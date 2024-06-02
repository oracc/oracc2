#!/bin/dash

lxd=01bld/lxinputs
project=`oraccopt`

if [ "$project" = "" ]; then
    >&2 echo $0: no project found. Stop.
    exit 1
fi

set 00atf/*.atf
if [ "$1" != "00atf/*.atf" ]; then
    cat 00atf/*.atf | atfdatax $project >02pub/atf-data.tab
elif [ -r 00lib/umbrella.lst ]; then
    lx -a 00lib/umbrella.lst -p $project
elif [ -r 00lib/search.lst ]; then
    lx -a 00lib/search.lst
else
    >&2 echo $0: no atf files found in project $project. Stop.
    exit 1
fi

set $lxd/proxy*.lst
if [ "$1" != "$lxd/proxy*.lst" ]; then
    >&2 ls -l $lxd/proxy*.lst
    if [ -r $lxd/proxy.lst ] || [ -r $lxd/proxy-cat.lst ]; then
	>&2   echo $0: processing $lxd/proxy*.lst
	lx -ui 02pub/atf-data.tab +? $lxd/proxy.lst +? $lxd/proxy-cat.lst
    fi
fi

chmod o+r 02pub/atf-data.tab
