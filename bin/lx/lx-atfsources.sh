#!/bin/dash
mkdir -p 01bld/lxlists
if [ -d 01bld/lxlists ]; then
    set 00atf/*.ods
    if [ "$1" != "00atf/*.ods" ]; then
	for a in $* ; do
	    ods2atf.sh $a
	done
    fi
    ls -1 00atf/*.atf >01bld/lxlists/atfsources.lst
fi
