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
for a in $sorts ; do
    ( cd ${ORACC_BUILDS}/$p/02pub/p4.d/outlined.lst/$a ;
      for t in itm max ; do
	  cat $t.tsv | dbix -s -d . -n $t
      done
      for t in pag ; do
	  tx -t $t.tsv
      done
    )
done
( cd ${ORACC_BUILDS}/$p/02pub ;
  for t in prx trs ; do
      if [ -r $t.tsv ]; then
	  cat $t.tsv | dbix -s -d . -n $t
      fi
  done
)
chmod -R o+r ${ORACC_BUILDS}/$p/02pub
