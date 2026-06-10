#!/bin/sh
#
# This script requires cdli.csv in 00src
#
# CDLI cat has sporadic control chars from other cat imports so we
# have to be very careful about line-end remapping.  We remove ^A from
# the input and then tell csvformat to use ^A as the line-end; then we
# remove all newline characters etc., and then remap the ^A back to \n.
#
# While we're at it, we remap tabs in the .csv to spaces because we
# are turning the .csv into .tsv.
#
#(cd 00src ;
#tr '	' '  ' <cdli.csv | csvformat -T -M '' | tr '\n\003\r' '     ' | tr '' '\n' >cdli.tsv )

(cd 00src ;
 if [ ! -r cdli.csv ]; then
     echo $0: I need 00src/cdli.csv to work on. Stop.
     exit 1
 fi

#
# Remove the following:
#
# ^A .. ^H ^N .. ^]
# tr -d '\001-\010\016-\035'
#
# Replace these with spaces:
#
# tabs, feeds, seps to spaces
#
# tr '\011-\015\036\037' '       '
#

 tr -d '\001-\010\016-\035' <cdli.csv | tr '\011' ' ' | \
     csvformat -T -M '' | tr '\011-\015\036\037' '       ' |
     tr '' '\n' >cdli.tsv

)
