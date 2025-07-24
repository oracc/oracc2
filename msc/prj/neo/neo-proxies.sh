#!/bin/sh
rm -f 01tmp/proxy.lst
lx -s -u -a 00lib/order.lst >02pub/atf-data.tab
grep '#lem' 02pub/atf-data.tab | cut -d@ -f1 >00lib/proxy.lst
cp 00lib/proxy.lst 01bld/lists/outlined.lst
