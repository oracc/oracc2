#!/bin/sh
#
# This script requires cdli.csv in 00src
#
# CDLI cat has sporadic control chars from other cat imports so
# we have to be very careful about line-end remapping:
(cd 00src ;
tr '' ' ' <cdli.csv | csvformat -T -M '' | tr '\n\r' '     ' | tr '' '\n' >cdli.tsv )
