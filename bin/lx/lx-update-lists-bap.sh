#!/bin/bash
#
# Implement o2-lst.plx update_lists() build-approved-policy
#
#set -x
echo $0 $project
opt=`oraccopt . build-approved-policy`

case $opt in
    ""|atf)
	lx -q -o${out_approved} -p${project} ${have_atf} \
	   -? $lxd/not-approved.lst +? $lxd/add-approved.lst
	;;
    catalog)
	lx -q -o${out_approved} -p${project} ${cat_ids} \
	   -? $lxd/not-approved.lst +? $lxd/add-approved.lst
	;;
    search|umbrella)
	# handled in lx-inputs.sh
	;;
    static)
	if [ ! -r ${out_approved} ]; then
	    touch ${out_approved}
	fi
	;;
    approved)
	echo "$0: 'approved' is no longer a valid value for 'build-approved-policy'."
	echo "$0: please edit $lxd/config.xml and change 'approved' to 'static'."
	;;
    *)
	echo "$0: unknown build-approved-policy value '$opt'"
	;;
esac

