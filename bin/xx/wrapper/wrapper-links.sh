#!/bin/dash
for a in `grep '^"' progtab.g | cut -d'"' -f2`; do
    (cd /home/oracc/bin; ln -f xsltx $a)
done
