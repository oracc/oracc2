#!/bin/dash
for a in `grep '^"' progtab.g | cut -d'"' -f2`; do
    (cd $ORACC_BUILDS/bin; 
     echo ln xsltx $a)
done
