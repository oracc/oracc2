#!/bin/sh
h=`hostname`
x=$1
sigx <01bld/project.sig | sort -u >01bld/sigoid.tab
echo calling oidx -q -a -x -p neo -i sux on build-oracc
oidx -q -a -x -p neo -I sux <01bld/sigoid.tab >$ORACC/oid/nxid.tab
(cd $ORACC/oid ; if [ -s nxid.tab ]; then mv nxid.tab xid.tab; fi)
