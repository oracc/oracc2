#!/bin/dash

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

if [ -r 00lib/proxy.lst ]; then
    lx -cuxq -p $project -o 01tmp/proxy.lst 00lib/proxy.lst
    lx -cui 02pub/atf-data.tab + 01tmp/proxy.lst
fi

chmod o+r 02pub/atf-data.tab
