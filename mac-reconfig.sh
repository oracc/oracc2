#!/bin/sh
. ~/orc/bin/oraccenv.sh
glibtoolize
aclocal
autoheader
automake -Wno-portability
autoconf
./configure --prefix=/Users/stinney/orc
#make
#make install
