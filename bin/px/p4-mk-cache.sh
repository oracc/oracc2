#!/bin/sh
if [ "$1" == "local" ]; then
    mkdir -v -p ~/p4.d
    echo 'ORACC_P4_CACHE=~/p4.d ; export ORACC_P4_CACHE' >>~/.profile
    echo
    echo $0: You must now log out and log back in again.
    echo
elif [ "$1" == "" ]; then
    if [ "$ORACC_BUILDS" == "" ]; then
	echo $0: must set ORACC_BUILDS first. Stop.
	exit 1
    else
	p4d=$ORACC_BUILDS/www/p4.d
	sudo mkdir -vp $p4d
	sudo chown www-data:oracc $p4d
	sudo chmod 755 $p4d
	sudo chmod ug+s $p4d
    fi
fi
