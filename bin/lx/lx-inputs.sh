#!/bin/dash
#
# Marshall the list inputs and clean/validate them, putting the
# validated versions in 01bld/lxinputs.
#
# We assume that 01bld, 01tmp, and 02pub all exist
#
lxd=01bld/lxinputs
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
xmd-ids.plx | lx -cuqs -p $project -o $lxd/00cat.lst -

# 00atf data
lx-aftdata.sh
cut -f1 02pub/atf-data.tab | lx -cuqs -p $project -o $lxd/00atf.lst -

# 00lib lists
for a in approved.lst add-approved.lst not-approved.lst
	 outlined.lst add-outlined.lst not-outlined.lst
	 rejected.lst ; do
    if [ -r 00lib/$a ]; then
	echo $0: marshalling 00lib/$a
	lx -cuqs -p $project -o $lxd/$a 00lib/$a
    fi
done

# lx -x arg uses proxy mode where missing cat field is qualified with
# project member of item rather than arg project:
#
# In project poel:
#
#	blms:P123456 => blms:P123456@blms
#
#	P123456@blms => poel:P123456@blms
#
# In proxy mode it makes no sense for an item to have neither project
# nor catalog field; that's an error.
#
if [ -r 00lib/proxy.lst ]; then    
    lx -cuqsx -p project -o $lxd/proxy.lst 00lib/proxy.lst
fi

# umbrella and search lists
#
# umbrella.lst doesn't have the top-level project in its members;
# search.lst members are full project paths
#
if [ -r 00lib/umbrella.lst ]; then
    rm -f 01tmp/00umbrella.lst
    >&2 echo $0: marshalling 00lib/umbrella.lst
    exec >>01tmp/00umbrella.lst
    lx-umbrella.sh umbrella $project
    lx -cuq -p $project -o $lxd/00umbrella.lst 01tmp/00umbrella.lst
elif [ -r 00lib/search.lst ]; then
    rm -f 01tmp/00search.lst
    >&2 echo $0: marshalling 00lib/search.lst
    exec >>01tmp/00search.lst
    lx-umbrella.sh search
    lx -cuqs -p $project -o $lxd/00search.lst 01tmp/00search.lst
fi
