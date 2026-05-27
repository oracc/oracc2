#!/bin/dash
. ${ORACC}/bin/odo-func.sh
odo_odo $0 $*

project=$odo_proj

odo_gloss_sub () {
    t=$1 ; shift
    for a in $* ; do
	echo odo-gloss-$t.sh $project $a
	odo-gloss-$t.sh $project $a
    done
}

##
## Set the task
##
taskorlang=0

if [ "$1" = "" ]; then
    subcommand=all
else
    for a in xml htm web all ; do
	if [ "$1" = "$a" ]; then
	    subcommand=$1
	    break;
	fi
    done
    if [ "$subcommand" = "" ]; then
	subcommand="all"
	taskorlang=1
    else
	shift 1
    fi
fi

##
## Set the languages; 20260526: this code serves to validate the tasks
## and languages requested but does not currently restrict the
## processing to, e.g., a single language because the args are ignored
## in the odo_gloss_sub scripts.
##
siglangx=`siglangx -l | tr '\n' ' '`
if [ "$1" = "" ]; then
    set $siglangx
else
    for g; do
	found=0
	for a in $siglangx ; do
	    if [ "$g" = "$a" ]; then
		found=1
		break
	    fi
	done
	if [ $found -eq 0 ]; then
	    if [ $taskorlang -eq 1 ]; then
		echo "$0: $g is not a glossary task or a $project language. Stop."
	    else
		echo "$0: $g is not a $project language. Stop."
	    fi
	    exit 1
	fi
    done
fi

##
## execute the task(s) for each language
##
echo "$0: task=$subcommand; glossaries($#)=$*; gx=$gx"

if [ "$gx" != "" ]; then
    xml=gx
else
    xml=xml
fi

case $subcommand in
    xml) 	odo_gloss_sub $xml $* ;;
    htm|html) 	odo_gloss_sub htm $* ;;
    web) 	odo_gloss_sub web $* ;;
    all)
	odo_gloss_sub $xml $* 
	odo_gloss_sub htm $*
	odo-gloss-htm-wrapup.sh
	odo_gloss_sub web $* 
	odo-gloss-web-wrapup.sh
	;;
esac
