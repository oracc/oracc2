#!/bin/dash
echo $0 $*

user=$1 ; shift
project=$2 ; shift
subcommand=$3
if [ "$subcommand" = "" ]; then
    subcommand=all
else
    shift
fi

set glossaries $*
siglangx=`siglangx`
if [ "$1" = "" ]; then
    glossaries=$siglangx
else
    for g; do
	found=0
	for a in $siglangx ; do
	    if [ "$g" = "$a" ]; then
		found=1
		break
	    fi
	done
	if [ "$found" = "0" ]; then
	    echo "$0: glossary $g not found in project $project. Stop."
	    exit 1
	fi
    done
fi

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
