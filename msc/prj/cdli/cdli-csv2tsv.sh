#!/bin/sh
# CDLI cat has sporadic control chars from other cat imports so
# we have to be very careful about line-end remapping:
tr '' ' ' <cdli.csv | csvformat -T -M '' | tr '\n\r' '     ' | tr '' '\n' >cdli.tsv
