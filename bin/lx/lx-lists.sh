#!/bin/sh
#
# create base lists from cat, atf, lib and umbrella/search in
# 01bld/lxinputs
#

#set -x

echo $0 $*

mkdir -p 01bld/lists
project=`oraccopt`
lx-inputs.sh

lxd=01bld/lists

# o2-lst.plx #1: a list of the .atf files in 00atf
lx-atfsources.sh

# o2-lst.plx #2: create_have_atf for  non-umbrella
if [ -r 02pub/atf-data.tab ]; then

   cut -f1 02pub/atf-data.tab | \
       lx -q -p $project - >$lxd/have-atf.lst

   grep '#lem' 02pub/atf-data.tab | cut -f1 | \
       lx -qz -p $project - >$lxd/have-lem.lst

   grep -v '#lem' 02pub/atf-data.tab | cut -f1 | \
       lx -qz -p $project - >$lxd/sans-lem.lst

fi

#o2-lst.plx #2': cat-ids.lst is redundantly created in o2-lst.sh and
#o2-lst.plx so we separate that out during cx implementation
if [ -s $lxd/00cat.sh ]; then
    cut -d@ -f1 $lxd/00cat.sh >$lxd/cat-ids.lst
fi

#o2-lst.plx #3: proxy_lists
lx-proxy-lists.sh

#o2-lst.plx #4: update_lists
lx-update-lists.sh

#o2-lst.plx #5: outlined_list
#
#This is probably unnecessary because lx takes care of preserving
#proxy project information as it generates outlined.lst
#
#lx-outlined-list.sh

#o2-lst.plx #6: lemindex_list
grep '#lem' ${ORACC}/$project/02pub/atf-data.tab | cut -d@ -f1 >$lxd/lemindex.lst

#if [ -r 01bld/lists/proxy.lst ]; then
#    lx -qous -p `oraccopt` 01bld/lists/proxy.lst >01bld/lists/outlined.lst
#    (cd 01bld/lists ; cp outlined.lst proxy-atf.lst ; cp outlined.lst proxy-cat.lst)
#fi
