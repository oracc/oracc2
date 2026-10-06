#!/bin/sh
grep '#lnk' 02pub/atf-data.tab | cut -d'@' -f1 | \
    scolinks >scolinks.xml 2>01tmp/scolinks.log
