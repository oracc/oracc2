#!/bin/sh
rm -f 01tmp/proxy.lst
lx -s -u -a 00lib/order.lst >02pub/atf-data.tab
grep '#lem' 02pub/atf-data.tab | cut -d@ -f1 >00lib/proxy.lst
