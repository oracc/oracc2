#!/bin/sh
me=`whoami`
if [ $me != "root" ]; then
    groups=`groups | grep -c sudo`
    if [ "$groups" = "1" ]; then
	echo sudo
    fi
fi
