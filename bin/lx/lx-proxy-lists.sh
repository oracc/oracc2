#!/bin/sh
#
# Implement o2-lst.plx proxy_lists()
#

#set -x
lxd=01bld/lists
echo $0 $*
project=`oraccopt`

if [ -s $lxd/proxy.lst ]; then
    proxy=$lxd/proxy.lst
    lx -p $project $proxy / $lxd/00atf.lst >$lxd/skip-atf.lst
    if [ -s $lxd/skip-atf.lst ]; then
	echo "$0: proxies duplicated in atf:"
	cat $lxd/skip-atf.lst
    fi
    lx -p $project $proxy - $lxd/00atf.lst >$lxd/proxy-atf.lst
    lx -p $project $proxy - $lxd/00cat.lst >$lxd/proxy-cat.lst
fi
