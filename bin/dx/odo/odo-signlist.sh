#!/bin/dash
#
# Perform build for a signlist project.
#
# odo always does an odo-update so there's no need to create the
# indices etc. here.
#
echo $0 $*
projtype=`oraccopt . type`
if [ "$projtype" = "corpus" ]; then
    odo-corpus-signlist.sh
else
    sxweb.sh
    tisdbi.sh
    odo-portal.sh
fi
