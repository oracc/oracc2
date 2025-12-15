#!/bin/sh
echo $0
shd=`pwd`
(cd ~/orc/osl ; ${ORACC_sudo} $shd/osl-user-site.sh)
