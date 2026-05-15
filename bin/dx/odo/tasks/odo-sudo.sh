#!/bin/sh
if [ "${ORACC_MODE}" = "multi" ]; then
    me=`whoami`
    if [ $me != "root" ]; then
	groups=`groups | grep -c sudo`
	if [ "$groups" = "1" ]; then
	    echo sudo
	fi
    fi
fi
