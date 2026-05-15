#!/bin/dash
. ${ORACC}/bin/odo-func.sh
odo_odo $0 $*
(cd .. ; find $1/00* -type f | xargs ls -l --time-style=long-iso)
