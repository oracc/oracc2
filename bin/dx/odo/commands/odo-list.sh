#!/bin/dash
. ${ORACC}/bin/odo-func.sh
odo_odo $0 $*
(cd ${ORACC} ; find $odo_proj/00* -type f | xargs ls -l --time-style=long-iso)
