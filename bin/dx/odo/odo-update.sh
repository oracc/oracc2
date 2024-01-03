#!/bin/dash
#

echo $0 $*

# update OIDs
mcu-oids.sh $*
if [ $? -ne 0 ]; then
    exit 1
fi

# update signlist
mcu-slix.sh $*
if [ $? -ne 0 ]; then
    exit 1
fi

# update lemm
mcu-lemm.sh lemm

exit $?
