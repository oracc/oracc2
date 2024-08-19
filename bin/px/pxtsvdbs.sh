#!/bin/sh
flds=`oraccopt . outline-special-sort-fields`
if [ "$flds" != "" ]; then
    spec=1
    perm=`/bin/echo -n $flds | tr -cd ,`
else
    flds=`oraccopt . outline-default-sort-fields`
    spec=0
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
	  dbix -s -d . -n $t <$t.tsv
      done
      for t in pag ; do
	  tx -t $t.tsv
      done
    )
done

# If we processed a special mode also rebuild txtindex.lst tsvs
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
    ( cd ${ORACC_BUILDS}/$p/02pub/p4.d/txtindex.lst/$a ;
      for t in itm max ; do
	  dbix -s -d . -n $t <$t.tsv
      done
      for t in pag ; do
	  tx -t $t.tsv
      done
    )
    done
fi

( cd ${ORACC_BUILDS}/$p/02pub ;
  for t in prx prx-cat trs ; do
      if [ -r $t.tsv ]; then
	  dbix -s -d . -n $t <$t.tsv
      fi
  done
)

( cd ${ORACC_BUILDS}/$p/02pub/cbd ;
  if [ -r oxid.tsv ]; then
      dbix -s -d . -n oxid <oxid.tsv
  fi
  for elst in */entry_ids.lst ; do
      l=`dirname $elst`
      (cd $l ;
       for t in etm map ; do
	   if [ -r $t.tsv ]; then
	       dbix -s -d . -n $t <$t.tsv
	   fi	   
       done
       if [ -r $l.tis ]; then
	   xisdb ./$l.tis
       fi
       )
  done
)
      
chmod -R o+r ${ORACC_BUILDS}/$p/02pub
