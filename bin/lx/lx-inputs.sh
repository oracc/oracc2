#!/bin/sh
#
# Marshall the list inputs and clean/validate them, putting the
# validated versions in 01bld/lxinputs.
#
# We assume that 01bld, 01tmp, and 02pub all exist
#

#set -x
#echo $0 $*

lxd=01bld/lists
if [ -d $lxd ]; then
    rm -f $lxd/*
else
    mkdir -p $lxd
fi

project=`oraccopt`
if [ "$project" = "" ]; then
    echo $0: unable to set project--probably not working in a project directory. Stop.
    exit 1
fi

# 00cat list
#xmd-ids.plx >01tmp/xmdids.lst
cx-xmd-ids.plx
if [ -s 01tmp/xmdids.lst ]; then
    lx -cus -p $project -o $lxd/00cat.lst 01tmp/xmdids.lst
fi

# 00lib lists
for a in approved.lst add-approved.lst not-approved.lst \
	 outlined.lst add-outlined.lst not-outlined.lst \
	 rejected.lst ; do
    if [ -s 00lib/$a ]; then
	echo $0: marshalling 00lib/$a
	lx -cusz -p $project -o $lxd/$a 00lib/$a
    fi
done

if [ -s 00lib/proxy.lst ]; then
    echo $0: marshalling 00lib/proxy.lst
    lx -cus -p $project -o $lxd/proxy.lst 00lib/proxy.lst
fi

# 00atf data; do this after 00lib because then it can use $lxd/proxy.lst
lx-atfdata.sh
cut -f1 02pub/atf-data.tab | lx -cus -p $project -o $lxd/00atf.lst -

# umbrella and search lists
#
# umbrella.lst doesn't have the top-level project in its members;
# search.lst members are full project paths
#
bap=`oraccopt . build-approved-policy`
if [ "$bap" == "proxy" ]; then
    cp $lxd/proxy.lst $lxd/outlined.lst
    cp $lxd/proxy.lst $lxd/proxy-atf.lst
elif [ -r 00lib/umbrella.lst ]; then
    rm -f 01tmp/00umbrella.lst
    >&2 echo $0: marshalling 00lib/umbrella.lst
    exec >>01tmp/00umbrella.lst
    lx-umbrella.sh umbrella $project
    lx -cu -p $project -o $lxd/00umbrella.lst 01tmp/00umbrella.lst
elif [ -r 00lib/search.lst ]; then
    rm -f 01tmp/00search.lst
    >&2 echo $0: marshalling 00lib/search.lst
    exec >>01tmp/00search.lst
    lx-umbrella.sh search
    lx -cus -p $project -o $lxd/00search.lst 01tmp/00search.lst
    if [ ! -s $lxd/outlined.lst ]; then
	cut -d'@' -f1 <$lxd/00search.lst >$lxd/outlined.lst
    fi
fi
