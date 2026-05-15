#!/bin/sh
. ${ORACC}/bin/odo-func.sh
odo_odo $0 $*

# Set blem so that lem is unset if odo-atf.sh check is called with no 'lem' arg
if [ -r .ax ]; then
    oxax=ax
    g=-g
    blem=-l
else
    oxax=oxx
    blem=-CD
fi

case $1 in
    good|ok)
	cklist=ax.ok
	shift
    ;;
    bad|no)
	cklist=ax.no
	shift
	;;
    build)
	lem=$blem
	;;
    check)
	chk=-c
	;;
    install)
	if [ "$proj" = "" ]; then
	    echo $0: proj not set. Stop.
	    exit 1
	else
	    echo $0: installing '00atf/*.atf'
	    ${ORACC}/bin/atfinstall -p $proj 00atf/*.atf
	    exit $?
	fi
	;;
    lem)
	lem=-m
	;;
    *)
	echo $0: bad argument $1. Stop.
	exit 1
    ;;
esac

if [ -d "00atf" ]; then
    if [ "$oxax" = "ax" ]; then
	${ORACC}/bin/ax $chk $lem $g -I01bld/lists/atfinstall.lst
    else
	batch=`oraccopt . atf-batch`
	if [ "$batch" = "yes" ]; then
	    for a in 01bld/atf-batch-*.lst ; do
		$oxax $args $g -I$a
	    done
	else
	    files=`find 00atf -maxdepth 1 -type f | grep '.\(ods\|.atf\)$' | grep -v ods.atf`
	    $oxax $args $files
	fi
    fi
fi
