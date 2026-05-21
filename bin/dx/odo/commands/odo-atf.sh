#!/bin/sh
. ${ORACC}/bin/odo-func.sh
odo_odo $0 $*
#set -x
#dry=yes

# Set blem so that lem is unset if odo-atf.sh check is called with no 'lem' arg
if [ -r .ax ]; then
    oxax=ax
    g=-g
    blem=-l
    clem=-l
    dflt=-l
    cklist=01bld/lists/atfinstall.lst
else
    oxax=oxx
    blem=-CD
    clem=-m
    dflt=-CD
fi

for a in $* ; do
    case $a in
	good|ok)
	    shift
	    cklist=ax.ok
	    ;;
	bad|no)
	    shift
	    cklist=ax.no
	    ;;
	build)
	    shift
	    cmd=build
	    ;;
	check)
	    shift
	    cmd=check
	    chk=-c
	    dflt=
	    ;;
	dry)
	    dry=yes
	    ;;
	install)
	    shift
	    if [ "$odo_proj" = "" ]; then
		echo $0: project not set. Stop.
		exit 1
	    else
		echo $0: installing '00atf/*.atf'
		${ORACC}/bin/atfinstall -p $odo_proj 00atf/*.atf
		exit $?
	    fi
	    ;;
	lem)
	    shift
	    if [ "$cmd" = "build" ]; then
		lem=$blem
	    else
		lem=$clem
	    fi
	    ;;
	val)
	    shift
	    val=-v
	    dflt=
	    ;;
	*)
	    if [ -r $a ]; then
		break
	    else
		odo_fail $0: bad argument $1. Stop.
	    fi
	    ;;
    esac
done

if [ "$dflt" != "" ]; then
    lem=$dflt
fi

if [ "$dry" = "yes" ]; then
    echo $0: dry run requested for:
    if [ "$oxax" = "ax" ]; then
	echo "${ORACC}/bin/ax $chk $val $lem $g -I$cklist"
    else
	echo "${ORACC}/bin/oxx $chk $val $lem $*"
    fi
else
    if [ -d "00atf" ]; then
	if [ "$oxax" = "ax" ]; then
	    ${obin}/ax $chk $val $lem $g -I01bld/lists/atfinstall.lst
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
fi
