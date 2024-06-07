#!/bin/dash
pid=`ps -ax -o pid,command | grep /dx$ | sed 's/^ //' | cut -d' ' -f1` 
if [ "$pid" != "" ]; then
    echo $0: killing PID $pid.
    kill $pid
else
    echo $0: no PID found for dx.
    if [ -r /tmp/oracc-daemon-dx ]; then
	echo $0: /tmp/oracc-daemon-dx still exists.  Will try to remove with sudo rm -f.
	sudo rm -fv /tmp/oracc-daemon-dx
	exit 0
    else
	exit 1
    fi
fi
