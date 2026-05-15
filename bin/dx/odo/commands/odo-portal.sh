##!/bin/dash
. ${ORACC}/bin/odo-func.sh
odo_odo $0 $*
exec o2-portal.sh $*
