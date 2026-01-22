#!/bin/bash
#
# Implement o2-lst.plx update_lists() build-outlined-policy
#

#set -x
echo $0 $project
opt=`oraccopt . build-outlined-policy`

case $opt in
    approved)
	lx -zq -p$project -o$out_outlined $out_approved \
	   -? $lxd/not-outlined.lst +? $lxd/add-outlined.lst \
	   +? $lxd/proxy-atf.lst
    ;;
    ""|atf)
	lx -q -p$project -o$out_outlined $have_atf \
	   -? $lxd/not-outlined.lst +? $lxd/add-outlined.lst \
	   +? $lxd/proxy-atf.lst
	;;
    P|Q)
	if [ ! -r $lxd/outlined.lst ]; then
	    if [ "${opt}" = "P" ]; then
		PQX=':[PX]'
	    else
		PQX=':Q'
	    fi
	    grep $PQX $out_approved | \
		lx -p$project -o$out_outlined - \
		   -? $lxd/not-outlined.lst +? $lxd/add-outlined.lst
	fi
	;;
    static)
	if [ ! -s $lxd/outlined.lst ] ; then
	    echo $0: build-outlined-policy=static but 00lib/outlined.lst empty/non-existent.
	    exit 1
	fi
	;;
    catalog)
	cp $lxd/cat-ids.lst $out_outlined	
	;;
    *)
	;;
esac
