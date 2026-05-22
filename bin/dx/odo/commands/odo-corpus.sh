#!/bin/dash
. ${ORACC}/bin/odo-func.sh
odo_odo $0 $*

odo-catalog.sh

## These two were in o2-cat but they belong in corpus really
##
## Update: looks like normlinks.tab is no longer used though the json
## build does build a version of it.

### atflinks.plx >01bld/atflinks.tab
### linknorm.plx 01bld/atflinks.tab >01bld/normlinks.tab

### This used to rebuild 01bld/project.sig but that should not be needed
### any more. It also handled batching, but with the odo architecture in
### which .atf are installed first and then xtf conversion is done in
### the individual text directories batching will not be used either.
###
###o2-atf.sh

# This is the new way of updating 02pub/lemm-*.sig
glosigx -p $proj

# TODO: need to handle .ods files either before this, or by installing
# them and then doing ods2atf in Makefile.atf
atfinstall -p $odo_proj 00atf/*.atf

odo-atf.sh build $*

odo-tok.sh

#o2-glo.sh
odo-gloss.sh xml

# 02www/inst currently holds signlist instances but could hold others in future
rm -fr 02www/inst

# o2-xtf.sh handles data derived from .xtf--the actual .xtf are created via atfmake.sh
o2-xtf.sh $*
qindex.sh $odo_proj
o2-web-corpus.sh
odo-signlist.sh
o2-weblive.sh
o2-finish.sh
