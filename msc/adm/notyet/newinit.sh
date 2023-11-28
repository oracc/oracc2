#!/bin/sh
cd ${ORACC_HOME}/$1
export -p ORACC ORACC_HOME
sudo -E -u $1 -s ${ORACC}/bin/oracc.sh init $*
