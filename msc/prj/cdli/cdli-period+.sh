#!/bin/sh
#
# Add the Oracc short-period and sort code to 00etc/period.tsv
#
cut -f2 00etc/period.tsv | cdliperiodx | paste 00etc/period.tsv - >00etc/period+.tsv

