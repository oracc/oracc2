#!/bin/sh
#
# create base lists from cat, atf, lib and umbrella/search in
# 01bld/lxinputs
#

#set -x

echo $0 $*

lx-inputs.sh

listdir=01bld/lxlists

# o2-lst.plx #1: a list of the .atf files in 00atf
lx-atfsources.sh

# o2-lst.plx #2: create_have_atf for  non-umbrella
if [ -r 02pub/atf-data.tab ]; then

   cut -f1 02pub/atf-data.tab | \
       lx -q -p `oraccopt` - >$listdir/have-atf.lst

   grep '#lem' 02pub/atf-data.tab | cut -f1 | \
       lx -qz -p `oraccopt` - >$listdir/have-lem.lst

   grep -v '#lem' 02pub/atf-data.tab | cut -f1 | \
       lx -qz -p `oraccopt` - >$listdir/sans-lem.lst

fi

#if [ -r 01bld/lists/proxy.lst ]; then
#    lx -qous -p `oraccopt` 01bld/lists/proxy.lst >01bld/lists/outlined.lst
#    (cd 01bld/lists ; cp outlined.lst proxy-atf.lst ; cp outlined.lst proxy-cat.lst)
#fi
