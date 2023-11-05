#!/bin/sh
check_env_or_die () {
    if [ "$2" = "" ]; then
	echo oracc-subst.sh: environment variable $1 is not set--add it to oraccenv.sh. Stop.
	exit 1
    fi
}

check_env_or_die ORACC_BUILDS $ORACC_BUILDS
check_env_or_die ORACC_HOME $ORACC_HOME
check_env_or_die ORACC_HOST $ORACC_HOST
check_env_or_die ORACC_MODE $ORACC_MODE
check_env_or_die ORACC_USER $ORACC_USER

xetex=`locate xetex | grep xetex$ | grep /bin/`
if [ "$xetex" == "" ]; then
    echo "$0: xetex not yet installed"
    TeXLive=missing-texlive
else
    echo "$0: xetex found as $xetex"
    TeXLive=`dirname $xetex`
fi

if [ -d /Users ]; then
    ORADMIN=/Users/oradmin
elif [ -d /home ]; then
    ORADMIN=/home/oradmin
else
    echo "$0: unable to set ORADMIN--no /home or /Users"
fi

export ORADMIN TeXLive

. ./oraccenv.sh ; printenv | grep 'ORACC\|ORADMIN\|TeXLive' | sed 's/=/@@#/' | sed 's/^/s#@@/' | sed 's/$/#/' >oracc.sed
