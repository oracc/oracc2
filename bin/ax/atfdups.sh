#!/bin/sh
#
# This script checks and optionally fixes duplicates as reported by
# the script o2/msc/prj/cdli/cdli-dups.sh--see that file for the
# format of the argument file for atfdups.sh
#
# Say atfdups.sh -f [DUPS_FILE] to fix 00atf/*.atf
#
bin=${ORACC}/bin

if [ "$1" = "-f" ]; then
    shift
    fix="yes"
fi

if [ "$1" = "" ]; then
    echo $0: must give dups file from cdli-dups.sh on command line. Stop.
    exit 1
fi

