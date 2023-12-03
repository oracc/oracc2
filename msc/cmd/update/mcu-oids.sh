#!/bin/dash
#
# mcu-oids.sh updates /home/oracc/oid from the github repo
#
# Are the .xml versions of the OIDs ever used? If so they could be
# rebuilt here.
#

if [ -d $ORACC_BUILDS/oid ]; then
    cd $ORACC_BUILDS/oid
    echo $0: updating OIDs--cwd=`pwd`
    # should check for stale .pulling lock file
    if [ ! -r .pulling ]; then
	touch .pulling
	runuser -u oradmin git pull
	rm -f .pulling
    fi
    exit 0
else
    echo $0: oids not yet installed--try osh oid init or the like. Stop.
    exit 1
fi
