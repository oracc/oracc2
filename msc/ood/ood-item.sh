#!/bin/dash
#
# helper script for P4 ispxsl.sh
#
# args: PROJECT OOD-ID OUTPUT-FILE-NAME
#
# OOD-ID should be the simple integer form; we take care of making the
# xml:id form with prefixed 'o' in this script
#
# (although it is overkill to have this simple command in a script,
# the ispxsl.sh architecture may grow to include other generated page
# types so it's better to encapsulate each generator neatly)
pub=${ORACC}/$1/02pub
if [ -d $pub/data.d ]; then
    cp -u $pub/data.d/o$2.xml $3
else
    ${ORACC}/bin/xfragx $pub/data.xml o$2 >$3
fi
