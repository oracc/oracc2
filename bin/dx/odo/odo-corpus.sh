#!/bin/dash
echo $0 $*
### o2-corpus.sh ###
o2-lst.sh
lx-lists.sh
o2-cat.sh
o2-atf.sh
if [ -r 01bld/destfiles.lst ]; then
    o2-tr-lst.plx <01bld/destfiles.lst >01bld/lists/have-xtr.tab
fi
buildpolicy=`oraccopt . build-approved-policy`;
if [ "$buildpolicy" != "search" ]; then
    umbrella=`oraccopt . cbd-super`;
    if [ "$umbrella" = "umbrella" ]; then
	echo "$0: getting sigs via umbrella.lst"
	echo '@fields sig inst' >01bld/from-prx-glo.sig
	for a in `cat 00lib/umbrella.lst` ; do
	    grep '%' $a/01bld/from-xtf-glo.sig >>01bld/from-prx-glo.sig
	done
    else
	echo "$0: getting sigs from corpus"
	[ -s 01bld/lists/have-lem.lst ] && l2p1-from-xtfs.plx -t 01bld/lists/have-lem.lst
	[ -s 01bld/lists/proxy-lem.lst ] && l2p1-from-xtfs.plx -proxy -t 01bld/lists/proxy-lem.lst
    fi
else
    echo "$0: getting sigs via search.lst"
    echo '@fields sig inst' >01bld/from-prx-glo.sig
    for a in `cat 00lib/search.lst` ; do
	xsig=${ORACC_BUILDS}/$a/01bld/from-xtf-glo.sig
	psig=${ORACC_BUILDS}/$a/01bld/from-prx-glo.sig
	if [ -r $xsig ]; then
	    grep '%' $xsig >>01bld/from-prx-glo.sig
	else
	    echo "search: no file $xsig"
	fi
	if [ -r $psig ]; then
	    grep '%' $psig >>01bld/from-prx-glo.sig
	else
	    echo "search: no file $psig"
	fi
    done
    sig-langs.sh 01bld/from-prx-glo.sig >01bld/superlangs
    /bin/echo -n ' qpn' >>01bld/superlangs
fi
o2-glo.sh
# 02www/inst currently holds signlist instances but could hold others in future
rm -fr 02www/inst
o2-xtf.sh $*
o2-web-corpus.sh
odo-signlist.sh
o2-weblive.sh
o2-finish.sh
### end of o2-corpus.sh ###
