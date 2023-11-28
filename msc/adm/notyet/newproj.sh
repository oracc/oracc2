#!/bin/sh
project=$1
if [ "$project" = "" ];
then
    echo newproj.sh: must give project name as argument
    exit 1
fi
if [ -d ${ORACC_HOME}/$project ];
then
    echo newproj.sh: user directory ${ORACC_HOME}/$project already exists
    exit 1
fi
if [ -d ${ORACC}/pub/$project ];
then
    echo newproj.sh: project directory ${ORACC}/pub/$project already exists
    exit 1
fi
echo creating user $project
if [[ $ORACC_MODE == "multi" ]]; then
    /usr/sbin/useradd -G oracc -m -k ${ORACC}/sbin/skel $project
    /usr/sbin/usermod -a -G $project oracc
fi
mkdir $ORACC_HOME/$project
cd ${ORACC_HOME}/$project
${ORACC}/sbin/newproj-dirs.sh $project
${ORACC}/sbin/fixuser.sh $project

if [[ $ORACC_MODE = "single" ]]; then
echo '**********************************************************************'
echo Now change directory to $ORACC_HOME/$project and use
echo   "oracc init ABBREV FULL PROJECT NAME"
echo to complete project creation.
echo '**********************************************************************'
else
echo '**********************************************************************'
echo Now use oman password $project to reset the password, then log into the
echo project and use
echo   "oracc init ABBREV FULL PROJECT NAME"
echo to complete project creation.
echo '**********************************************************************'
fi
exit 0
