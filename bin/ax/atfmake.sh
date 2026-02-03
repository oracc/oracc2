#!/bin/sh
atfargs=`oraccopt . atf-flags` ; export atfargs
project=`oraccopt` ; export project
cat 01bld/lists/atfinstall.lst | while read dir ; do
    pqx=`basename $dir` ; export pqx
    make -r -C $dir -f ${ORACC}/lib/data/Makefile.atf $1
done
