#!/bin/sh
. ./oraccenv.sh
libtoolize
aclocal
autoheader
automake -Wno-portability
autoconf
./configure --prefix=$ORACC
#make CFLAGS=-g
#make install
