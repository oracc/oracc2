#!/bin/dash
echo $0 $*

odo_gloss_sub () {
    t=$1 ; shift
    for a in $* ; do
#	echo odo-gloss-$t.sh $project $a
	odo-gloss-$t.sh $project $a
    done
}

##
## Set the task
##
taskorlang=0
user=$1
project=$2
if [ "$3" = "" ]; then
    subcommand=all
    shift 2
else
    for a in xml htm web all ; do
	if [ "$3" = "$a" ]; then
	    subcommand=$3
	    break;
	fi
    done
    if [ "$subcommand" = "" ]; then
	subcommand="all"
	taskorlang=1
	shift 2
    else
	shift 3
    fi
fi

##
## Set the languages
##
siglangx=`siglangx`
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
echo "$0: task=$subcommand; glossaries($#)=$*"

case $subcommand in
    xml) 	odo_gloss_sub xml $* ;;
    htm|html) 	odo_gloss_sub htm $* ;;
    web) 	odo_gloss_sub web $* ;;
    all)
	odo_gloss_sub xml $* 
	odo_gloss_sub htm $*
	odo-gloss-htm-wrapup.sh
	odo_gloss_sub web $* 
	odo-gloss-web-wrapup.sh
	;;
esac
