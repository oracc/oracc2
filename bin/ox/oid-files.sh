#!/bin/dash

#
# create an XML index of the OID files in directory $1
#

d=$1
x=$2

if [ "$d" != "" ] && [ "$x" != "" ]; then
    (cd $d ;
     printf "<provides dir=\"%s\" ext=\"%s\">" $d $x
     ls -1 [ox][0-9][0-9][0-9][0-9][0-9][0-9][0-9].$x | cut -d. -f1 | rocox -x - -R '<ox xml:id="%1"/>'
     printf "</provides>"
    )
else
    echo $0: must give directory and extension on command line. Stop.
fi
    
