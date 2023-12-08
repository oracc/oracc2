#!/bin/dash
echo $0 $*

subcommand=$3

case $subcommand in
    xml) odo-gloss-xml.sh $* ;;
    htm) odo-gloss-htm.sh $* ;;
    html) odo-gloss-htm.sh $* ;;
    web) odo-gloss-web.sh $* ;;
    *)
	odo-gloss-xml.sh $* 
	odo-gloss-htm.sh $* 
	odo-gloss-web.sh $* 
	;;
esac
