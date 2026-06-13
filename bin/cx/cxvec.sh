#!/bin/sh
catpnums=${ORACC}/lib/data/catpnums.vec
cxvec 01bld/lists/cat-ids.lst >$catpnums
chmod o+r $catpnums
