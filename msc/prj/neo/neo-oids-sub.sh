#!/bin/sh
l=$1
echo calling oidx -a -x -p neo -d $l on build-oracc
oidx -a -x -p neo -d $l <01bld/$l/need-oid.tab >xid.tab
