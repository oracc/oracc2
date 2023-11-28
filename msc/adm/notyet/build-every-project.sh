#!/bin/sh
if [ $(whoami) = "root" ]; then
    echo build-all-projects.sh: rebuilding all PUBLIC projects
    for a in `${ORACC}/bin/agg-list-public-projects.sh` ; do
	user=`echo -n $a | sed 's,/.*$,,'`
	sudo -u $user -s oracc build
    done
else
    echo build-all-projects.sh: this program can only be run as root
fi
