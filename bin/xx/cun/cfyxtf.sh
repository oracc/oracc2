#!/bin/sh
#
# Read a list of .xtf files and run cfy on each one
# with supplied args
#
declare -i don=0
declare -i bad=0
while read xtf ; do
    >&2 echo $xtf
    don+=1
    # pass args to cfy; use -n to send output to /dev/null
    cfy $* $xtf
    if [ $? -ne 0 ]; then
	bad+=1
    fi
done

echo $0: $don files processed / $bad exiting with error.
