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
(cd 00src ;
tr '	' '  ' <cdli.csv | csvformat -T -M '' | tr '\n\r' '     ' | tr '' '\n' >cdli.tsv )
