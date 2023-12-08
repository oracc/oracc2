#!/bin/dash
echo $0 $*
case $1 in
    xml) odo-gloss-xml.sh ;;
    htm) odo-gloss-htm.sh ;;
    html) odo-gloss-htm.sh ;;
    install) odo-gloss-web.sh ;;
    *)
	odo-gloss-xml.sh
	odo-gloss-htm.sh
	odo-gloss-web.sh
	;;
esac
