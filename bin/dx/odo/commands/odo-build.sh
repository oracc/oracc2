#!/bin/dash
#
# osh PROJECT build calls this script with odo_user and odo_proj set
# in the environment.
#
#set -x

. ${ORACC}/bin/odo-func.sh
odo_odo $0 $*

rm -f 01bld/cancel

if [ "$1" = "" ]; then
    projtype=`oraccopt . type`
    subcommand=$projtype
else
    subcommand=$1 ; shift
fi

touch .oracc-build ; chmod 0644 .oracc-build

case $subcommand in
    catalog|catalogue) 	odo-catalog.sh $* ;;
    corpus) 	odo-corpus.sh $* ;;
    gloss) 	odo-gloss.sh $* ;;
    ood) 	odo-ood.sh $* ;;
    portal) 	odo-portal.sh $* ;;
    lexdata)    odo-lexdata.sh $* ;;
    signlist)	odo-signlist.sh $* ;;
    *)		odo-build-default.sh $1 $2 $subcommand $4 $5 ;;
esac
