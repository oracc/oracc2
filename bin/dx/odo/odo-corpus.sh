#!/bin/dash
echo $0 $*
user=$1
proj=$2
### o2-corpus.sh ###
# o2-lst.sh ## should be fully replaced by lx-lists.sh and cx(-marshall).sh
# lx-lists.sh ## this is done in odo-catalog.sh
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
atfinstall -p $proj 00atf/*.atf

# This is the new way of making .xtf and other derivatives of .atf files:
# .xtf and .tok files are the default products.
atfmake.sh

# This is the new way of generating corpus statistics from .tok files.
atftoks.sh

### should be obviated by atf-data.tab:
### if [ -r 01bld/destfiles.lst ]; then
###    o2-tr-lst.plx <01bld/destfiles.lst >01bld/lists/have-xtr.tab
###fi

###
### This should all be obviated by atfdata.tab and tokfiles.lst
###
# # buildpolicy=`oraccopt . build-approved-policy`;
# # if [ "$buildpolicy" != "search" ]; then
# #     umbrella=`oraccopt . cbd-super`;
# #     if [ "$umbrella" = "umbrella" ]; then
# # 	echo "$0: getting sigs via umbrella.lst"
# # 	echo '@fields sig inst' >01bld/from-prx-glo.sig
# # 	for a in `cat 00lib/umbrella.lst` ; do
# # 	    grep '%' $a/01bld/from-xtf-glo.sig >>01bld/from-prx-glo.sig
# # 	done
# #     else
# # 	echo "$0: getting sigs from corpus"
# # 	[ -s 01bld/lists/have-lem.lst ] && l2p1-from-xtfs.plx -t 01bld/lists/have-lem.lst
# # 	[ -s 01bld/lists/proxy-lem.lst ] && l2p1-from-xtfs.plx -proxy -t 01bld/lists/proxy-lem.lst
# #     fi
# # else
# #     echo "$0: getting sigs via search.lst"
# #     echo '@fields sig inst' >01bld/from-prx-glo.sig
# #     for a in `cat 00lib/search.lst` ; do
# # 	xsig=${ORACC_BUILDS}/$a/01bld/from-xtf-glo.sig
# # 	psig=${ORACC_BUILDS}/$a/01bld/from-prx-glo.sig
# # 	if [ -r $xsig ]; then
# # 	    grep '%' $xsig >>01bld/from-prx-glo.sig
# # 	else
# # 	    echo "search: no file $xsig"
# # 	fi
# # 	if [ -r $psig ]; then
# # 	    grep '%' $psig >>01bld/from-prx-glo.sig
# # 	else
# # 	    echo "search: no file $psig"
# # 	fi
# #     done
# #     sig-langs.sh 01bld/from-prx-glo.sig >01bld/superlangs
# #     /bin/echo -n ' qpn' >>01bld/superlangs
# # fi

o2-glo.sh

# 02www/inst currently holds signlist instances but could hold others in future
rm -fr 02www/inst

# o2-xtf.sh handles data derived from .xtf--the actual .xtf are created via atfmake.sh
o2-xtf.sh $*
qindex.sh ${proj}
o2-web-corpus.sh
odo-signlist.sh
o2-weblive.sh
o2-finish.sh
### end of o2-corpus.sh ###
