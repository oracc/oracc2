#!/bin/sh
if [ "$1" = "" ]; then 
  echo delproj.sh: must give project name as first argument
  exit 1
fi
if [ "$ORACC_MODE" = "multi" ]; then
    home=/home
else
    home=$ORACC_HOME
fi
if [ ! -d $home/$1 ]; then
    echo delproj.sh: project $1 has no home, no tarball will be created
else
    mkdir -p /deleted_home
    cd $home 
    tar Jcf /deleted_home/$1.tar.xz $1
fi
if [[ $ORACC_MODE = "multi" ]]; then
    /usr/sbin/userdel $1
    /usr/sbin/groupdel $1
fi
chattr -i $home/$1/.bash*
chattr -a $home/$1/.bash*
rm -fr $home/$1
rm -fr ${ORACC}/$1
rm -fr ${ORACC}/{bld,pub,tmp,www,xml}/$1
