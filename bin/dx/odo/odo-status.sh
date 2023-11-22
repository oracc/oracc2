#!/bin/dash
#
# odo-status.sh: produce a simple status report for the project;
# mostly exists to have a vacuous exercise to carry out when testing
# dx/osh/odo cycle.

lastmod ()
{
    ls -clt --time-style=long-iso $1 | cut -d' ' -f6,7
}

echo Status report for project $2 as of `date +%Y-%m-%dT%T%Z`:

if [ -r .git ]; then
    if [ -r .oracc-git ]; then
	t=lastmod .oracc-git
	echo "--git repository last refreshed $t"
    else
	echo "--git repository never refreshed"
    fi
else
    echo "--not a git repository."
fi

if [ -r .oracc-build ]; then
    t=`lastmod .oracc-build`
    echo "--oracc build last done $t"
    if [ -r .oracc-serve ]; then
	t=lastmod .oracc-serve
	echo "--oracc serve last done $t"
    else
	echo "--never served"
    fi
else
    echo "--never built"
fi

echo End of status report.

exit 0
