#!/bin/bash
#
# Implement o2-lst.plx update_lists() build-outlined-policy
#

#set -x

. ${ORACC}/bin/odo-func.sh
odov $0 $*

opt=`oraccopt . build-outlined-policy`

if [ "$opt" = "" ]; then
    opt=`oraccopt . build-approved-policy`
fi

case $opt in
    approved)
	lx -qz -p$project -o$out_outlined -- $out_approved \
	   -? $lxd/not-outlined.lst +? $lxd/add-outlined.lst \
	   +? $lxd/proxy-atf.lst
    ;;
    atf)
	lx -qz -p$project -o$out_outlined -- $have_atf \
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
		lx -p$project -o$out_outlined -- - \
		   -? $lxd/not-outlined.lst +? $lxd/add-outlined.lst
	fi
	;;
    static)
	if [ ! -s $lxd/outlined.lst ] ; then
	    echo $0: build-outlined-policy=static but 00lib/outlined.lst empty/non-existent.
	    exit 1
	fi
	;;
    catalog|catalogue)
	cp $lxd/cat-ids.lst $out_outlined	
	;;
    *)
	echo $0: no build-outlined-policy set in 00lib/config.xml--pager will fail
	exit 1
	;;
esac
