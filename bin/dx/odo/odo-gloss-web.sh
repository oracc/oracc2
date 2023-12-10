#!/bin/bash
echo $0 $*
p=$1
l=$2
dwww=${ORACC_BUILDS}/$p/02www/cbd
mkdir -p $dwww
dbld=${ORACC_BUILDS}/$p/01bld/www/cbd/$l

(cd $dwww ;
 echo '>>>'Installing glossary $l in `pwd`
 if [ -d $l ]; then
	 mv -v $l x$l ; mv -v $dbld . ; rm -fr x$l
     else
	 mv -v $dbld .
 fi)
