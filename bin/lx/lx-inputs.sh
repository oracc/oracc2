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

# 00cat list

xmd-ids.plx | lx -cu -o $lxd/00cat.lst 01tmp/xmdids.lst

# 00atf list
cat 00atf/*.atf | atfdatax >02pub/atf-data.tab
cut -f1 02pub/atf-data.tab | lx -cu -o $lxd/00atf.lst -

# 00lib lists
for a in approved.lst not-outlined.lst outlined.lst proxy.lst rejected.lst ; do
    lx -cu -o $lxd/$a 00lib/$a
done

# umbrella and search lists
#
# umbrella.lst doesn't have the top-level project in its members;
# search.lst members are full project paths
#
if [ -r 00lib/umbrella.lst ]; then
    exec >>01tmp/00umbrella.lst
    lx-umbrella.sh 00lib/umbrella.lst `oraccopt`
    lx -cu -o $lxd/00umbrella.lst 01tmp/00umbrella.lst
elif [ -r 00lib/search.lst ]; then
    exec >>$01tmp/00search.lst
    lx-umbrella.sh 00lib/search.lst    
    lx -cu -o $lxd/00search.lst 01tmp/00search.lst
fi

