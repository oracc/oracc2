#!/bin/sh
flds=`oraccopt . outline-special-sort-fields`
if [ "$flds" != "" ]; then
    perm=`/bin/echo -n $flds | tr -cd ,`
else
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
    ls -ld outlined.lst
    echo $0: issx -C${ORACC_BUILDS} -P -r $a -j $p
    issx -C${ORACC_BUILDS} -P -r $a -j $p
    ls -ld outlined.lst
    ( cd ${ORACC_BUILDS}/$p/02pub/p4.d/outlined.lst/$a ; tx -u -d . -n itm >itm.tsv )
done
ls -ld outlined.lst
