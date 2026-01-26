#!/bin/sh
./tokx -L -p dcclt <~/orc/dcclt/01bld/lists/lemindex.lst >dcclt.tok
./tokexl -p dcclt dcclt.tok >dcclt.tkx
./tokix -d 02pub/tokl -p dcclt dcclt.tkx >dcclt.tix
