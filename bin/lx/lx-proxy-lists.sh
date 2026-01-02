#!/bin/sh
#
# Implement o2-lst.plx proxy_lists()
#

#set -x
echo $0 $*
project=`oraccopt`

if [ -s 01bld/lxinputs/proxy.lst ]; then
    proxy=01bld/lxinputs/proxy.lst
    lx -p $project $proxy / 01bld/lxinputs/00atf.lst >01bld/lists/skip-atf.lst
    if [ -s 01bld/lists/skip-atf.lst ]; then
	echo "$0: proxies duplicated in atf:"
	cat 01bld/lists/skip-atf.lst
    fi
    lx -p $project $proxy - 01bld/lxinputs/00atf.lst >01bld/lists/proxy-atf.lst
    lx -p $project $proxy - 01bld/lxinputs/00cat.lst >01bld/lists/proxy-cat.lst
fi
