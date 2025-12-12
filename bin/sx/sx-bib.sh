#!/bin/sh
##
## process the signlist XML to add bib data
##
refs=01tmp/references.div
bx -l ${refs} -f sl.xml >slb.xml
if [ $? -eq 0 ]; then
    mv slb.xml sl.xml
    bx -s -d ${refs} 01tmp/refs.bib
    xmllint --encode UTF-8 --xinclude --noxincludenode \
	    00etc/references.xml >00web/references.xml
fi
