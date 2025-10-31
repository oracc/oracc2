#!/bin/sh
if [ "$1" == "" ]; then
    echo $0: must give output file name on command line. Stop.
    exit 1
fi
exec 3>$1
d=${ORACC}/lib/bib
cd $d
for b in *.bib ; do
    echo '\\'"addbibresource{$d/$b}"
done >&3
