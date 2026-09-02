#!/bin/dash
#
# This script is run by odo before running any external odo commands
# (e.g., build, update, serve).
#
# It can also be run on its own with osh PROJECT config.
#
. ${ORACC}/bin/odo-func.sh
odo_odo $0 $*
odo_verbose $0 $1

project=$odo_proj
V=$odo_verb

#set -x
if [ -r 00lib/subconfig.xml ]; then
    echo $0: found 00lib/subconfig.xml
    sub="sub"
fi

if [ ! -r 00lib/${sub}config.xml ]; then
    echo $0: must be run from a project directory
    exit 1
fi

odo-dir-links.sh

if [ -r 00lib/subconfig.xml ]; then
    if [ "$odo_verb" != "" ]; then
	$0: creating 02xml/config.xml from 00lib/subconfig.xml
    fi
    xsltproc -o 02xml/config.xml $ORACC/lib/scripts/mcc-subconfig.xsl \
	00lib/subconfig.xml
else
    $odo_sudo cp $odo_verb 00lib/config.xml 02xml/config.xml
fi

set 00lib/config-*.xml

if [ "$1" != "00lib/config-*.xml" ]; then
    $odo_sudo cp $V 00lib/config-*.xml 02xml/
fi

if [ -r 00lib/pll.xml ]; then
    $odo_sudo cp $V 00lib/pll.xml 02xml/
fi

$odo_sudo cp $V 02xml/config.xml 02www

$odo_sudo chmod $V -f o+r 02xml/config.xml 02www/config.xml

if [ -r 00lib/names.xml ]; then
    $odo_sudo cp $V 00lib/names.xml 02pub
    $odo_sudo chmod $V o+r 02pub/names.xml
fi

# project-config is built in here even though it is controlled for
# other commands by odo
if [ -r $ORACC_BUILDS/$project-config.sh ]; then
    if [ "$V" != "" ]; then
	$0: running $ORACC_BUILDS/${project}-config.sh
    fi
    $ORACC_BUILDS/${project}-config.sh
fi

# The cx reimplementation of xmdmanager works in two phases; local
# catalogue marshalling is done with the following script; the build
# is done in odo-catalog.sh via cx.sh
#
# This phase must precede lx-lists.sh
#
${obin}/cx-marshall.sh

exit 0
