#!/bin/sh
(cd $ORACC ;
 for d in bld log pub tmp www xml ; do
     cd=`pwd`
     if [ ! -d $d/neo ]; then
	 echo "$0: no directory $d/neo in $cd; do"
	 echo "	(cd $ORACC/neo ; mkdir -p $d/neo)"
	 echo Stop.
	 exit 1
     fi
 done
 )
(cd $ORACC/neo
 sudo rm -fr 01* 02*
 sudo ln -sf $ORACC/bld/neo 01bld
 sudo ln -sf $ORACC/log/neo 01log
 sudo ln -sf $ORACC/tmp/neo 01tmp
 sudo ln -sf $ORACC/pub/neo 02pub
 sudo ln -sf $ORACC/www/neo 02www
 sudo ln -sf $ORACC/xml/neo 02xml
)


