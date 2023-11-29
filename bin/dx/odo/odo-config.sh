#!/bin/dash
#
# This script is run by odo before running any external odo commands
# (e.g., build, update, serve).
#
# It can also be run on its own with osh PROJECT config.
#

echo $0 $*

project=$1
V=$2

if [ ! -r 00lib/config.xml ]; then
    echo $0: must be run from a project directory
    exit 1
fi

if [ -r 00lib/subconfig.xml ]; then
    if [ "$V" != "" ]; then
	$0: creating 02xml/config.xml from 00lib/subconfig.xml
    fi
    xsltproc -o 02xml/config.xml $ORACC/lib/scripts/mcc-subconfig.xsl \
	00lib/subconfig.xml
else
    cp $V 00lib/config.xml 02xml/config.xml
fi

set 00lib/config-*.xml

if [ "$1" != "00lib/config-*.xml" ]; then
    cp $V 00lib/config-*.xml 02xml/
fi

if [ -r 00lib/pll.xml ]; then
    cp $V 00lib/pll.xml 02xml/
fi

cp $V 02xml/config.xml 02www

chmod $V -f o+r 02xml/config.xml 02www/config.xml

if [ -r 00lib/names.xml ]; then
    cp $V 00lib/names.xml 02pub
    chmod $V o+r 02pub/names.xml
fi

# project-config is built in here even though it is controlled for
# other commands by odo
if [ -r $ORACC_BUILDS/$project-config.sh ]; then
    if [ "$V" != "" ]; then
	$0: running $ORACC_BUILDS/${project}-config.sh
    fi
    $ORACC_BUILDS/${project}-config.sh
fi

exit 0
