#!/bin/sh
catpnums=02pub/catpnums.vec
cxvec 01bld/lists/cat-ids.lst >$catpnums
chmod o+r $catpnums
