#!/bin/sh
echo $0
if [ "$ORACC_MODE"="multi" ]; then
   sudo=sudo
fi
shd=`pwd`
(cd ~/orc/osl ; $sudo $shd/osl-user-site.sh)
