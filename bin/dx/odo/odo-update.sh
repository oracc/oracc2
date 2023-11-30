#!/bin/dash
#

echo $0 $*

# update OIDs
mcu-oids.sh

# update signlist
mcu-slix.sh

# update lemm
mcu-lemm.sh
if [ -r 01log/glo.err ]; then
    for a in `cat 01log/glo.err` ; do
	cat $a
    done
fi
