#!/bin/sh
if [ "$UID" != "0" ]; then
    echo "$0: Must be root"
    exit 1
fi

if [ "$1" = "" ]; then
    echo "$0: Must give project as argument to fixuser.sh"
    exit 1
fi

if [[ $ORACC_MODE = "single" ]]; then
    FUHOME=$ORACC_HOME
    ouser=$(cat $ORACC_BUILDS/.oraccuser)
    ogroup=$(cat $ORACC_BUILDS/.oraccgroup)
else
    FUHOME=/home
    ouser=$1
    ogroup=$1
fi

if [ ! -d $FUHOME/$1 ]; then
    echo "$0: No home directory for $1"
    exit 1
fi
cd $FUHOME/$1
if [[ ! $ORACC_MODE = "single" ]]; then
    chattr -i .bash*
    chattr -a .bash*
    chown $ouser:$ogroup .bash*
    chmod go-rwx .bash*
    chmod u-w .bashrc .bash_profile
    chattr +i .bash_profile .bashrc
    chattr -f +a .bash_history
fi
chmod +s .
