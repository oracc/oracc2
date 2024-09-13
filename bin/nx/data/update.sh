#!/bin/sh
for a in ed3a ed3b oakk lagash2 ur3 oldbab ; do
    numtokx <$ORACC/epsd2/admin/$a/01bld/lists/lemindex.lst >$a.num
    ./gooduns.sh <$a.num >$a.good
    ./refinvert.sh num.ref $a.tab
done
rm -f num.ref
