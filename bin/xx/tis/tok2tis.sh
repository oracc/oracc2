#!/bin/sh
./tokex <dcclt.tok  >dcclt.tkx
./tokix -c tok.cnt -d . <dcclt.tkx 
sort tok.cnt | cut -f 2 | paste tid.tsv - >col3.tis
