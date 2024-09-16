#!/bin/sh
d="S C"
for a in $d; do
    (cd $a;
     for b in *.in ; do
	 o=`basename $b in`out
	 ../../nx -d ../../ns.dat <$b | diff -q $o -
	 if [ $? != 0 ]; then
	     echo $a/$b failed
	 else
	     echo $a/$b ok
	 fi
     done
     )
done
