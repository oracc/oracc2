#!/bin/sh
rm -f 01bld/atflink.tab
for s in . `list-all-subs.sh`; do
    set $s/00atf/*.atf
    if [ "$*" != "$s/00atf/*.atf" ];l then
       cat $* | atflinkx >>01bld/atflink.tab
    fi
fi
