#!/bin/sh
. ${ORACC}/bin/odo-func.sh
odo_odo $0 $*
echo "$0 odo_user=$odo_user; odo_proj=$odo_proj; odo_verb=$odo_verb"
