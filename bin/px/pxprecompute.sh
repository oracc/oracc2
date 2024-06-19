#!/bin/sh
flds=`oraccopt . outline-special-sort-fields`
if [ "$flds" != "" ]; then
    spec=1
    perm=`/bin/echo -n $flds | tr -cd ,`
else
    spec=0
    flds=`oraccopt . outline-default-sort-fields`
    if [ "$flds" != "" ]; then
	perm=`/bin/echo -n $flds | tr -cd ,`	
    else
	perm=,,
    fi
fi
if [ "$perm" = ",," ]; then
    sorts="123 132 213 231 312 321"
elif [ "$perm" = "," ]; then
    sorts="12 21"
else
    sorts="1"
fi
p=`oraccopt`
rm -fr 02pub/p4.d
mkdir -p 02pub/p4.d

for a in $sorts ; do
    echo $0: issx -C${ORACC_BUILDS} -P -r $a -j $p
    issx -C${ORACC_BUILDS} -P -r $a -j $p
    ( cd ${ORACC_BUILDS}/$p/02pub/p4.d/outlined.lst/$a ; tx -u -d . -n itm >itm.tsv )
done

# If we processed a special mode also precompute a default mode based on txtindex.lst
if [ "$spec" = "1" ]; then
    flds=`oraccopt . outline-default-sort-fields`
    if [ "$flds" != "" ]; then
	perm=`/bin/echo -n $flds | tr -cd ,`	
    else
	perm=,,
    fi
    if [ "$perm" = ",," ]; then
	sorts="123 132 213 231 312 321"
    elif [ "$perm" = "," ]; then
	sorts="12 21"
    else
	sorts="1"
    fi
    for a in $sorts ; do
	echo $0: issx -C${ORACC_BUILDS} -P -r $a -j $p -l txtindex.lst
	issx -C${ORACC_BUILDS} -P -r $a -j $p -l txtindex.lst
	( cd ${ORACC_BUILDS}/$p/02pub/p4.d/txtindex.lst/$a ; tx -u -d . -n itm >itm.tsv )
    done
fi
