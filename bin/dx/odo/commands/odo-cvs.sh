#!/bin/dash
. ${ORACC}/bin/odo-func.sh
odo_odo $0 $*

odo-cvs-refresh.sh

exit $?
