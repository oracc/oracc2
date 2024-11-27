#!/bin/dash
echo $0
set 00lib/*.asl
if [ "$1" != "00lib/*.asl" ]; then
    if [ "$2" = "" ]; then
	asl=$1
	project=`grep ^@project $asl | tr -s '\t ' ' ' | cut -d' ' -f2`
	if [ "$project" != "" ]; then
	    echo $0: updating indexes for $asl in project $project
	    sx -P$project -c $asl
	    if [ $? -eq 0 ]; then
		sx -P$project -s $asl >02pub/sl/sl.tsv
		slix 02pub/sl/sl.tsv
	    else
		echo $0: errors checking $asl. Stop.
		exit 1
	    fi
	else
	    echo $0: no project found in $asl. Stop.
	    exit 1
	fi
    else
	echo $0: there should only be one signlist but I see $*. Stop.
	exit 1
    fi	
else
    echo "$0: I only work with a static signlist in 00lib, e.g., 00lib/pcsl.asl or 00lib/osl.asl. Stop."
    exit 1
fi
