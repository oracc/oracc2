#!/bin/bash
#
# Implement o2-lst.plx update_lists()
#
# Here and elsewhere in the shell port of o2-lst.plx we abandon the
# once-intended support for .ol files which would have provided a way
# of creating lists from project names and rules.  This facility
# hasn't been needed for the first 15 years of Oracc so we can clearly
# live without it.
#

#set -x
set -a
echo $0 $*
bin=${ORACC}/bin

project=`oraccopt`
if [ "${project}" = "" ]; then
    echo $0: must be called from project directory. Stop.
    exit 1
fi

lxd=01bld/lists
cat_ids="$lxd/cat-ids.lst"
have_atf="$lxd/have-atf.lst"
have_lem="$lxd/have-lem.lst"
sans_lem="$lxd/sans-lem.lst"
have_xtf="$lxd/have-xtf.lst"
out_approved="$lxd/approved.lst"
out_outlined="$lxd/outlined.lst"

# build-approved-policy
$bin/lx-update-lists-bap.sh

# build-outlined-policy
$bin/lx-update-lists-bop.sh $out_outlined

cp $have_atf $lxd/withatf
lx $out_outlined - $have_atf >$lxd/sansatf
cp $have_lem $lxd/withlem
cp $sans_lem $lxd/sanslem
