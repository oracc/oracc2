#!/bin/sh
h=`hostname`
x=$1
sigx <01bld/project.sig | sort -u >01bld/sigoid.tab
echo calling oidx -a -x -p neo -i sux on build-oracc
oidx -a -x -p neo -I sux <01bld/sigoid.tab >xid.tab
