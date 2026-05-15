#!/bin/sh
. ${ORACC}/bin/odo-func.sh
odo_odo $0 $*
o2-harvest.sh $*
