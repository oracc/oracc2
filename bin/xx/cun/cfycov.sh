#!/bin/bash
#
# Use cfy to test coverage of a text:
#
function usage {
    cat <<EOF
Usage: cfycov.sh -u UNILIST [-p PROJECT | -q QUALTEXT | -t TEXTLIST ]

	-u UNILIST : list of unicode characters to test coverage against
	-p PROJECT : check all texts in named PROJECT
	-q QUALTEXT : check coverage of a the given single text
	              QUALTEXT is PROJECT:PQXID, e.g., epsd2/admin/ed3a:P011110
	-t TEXTLIST : check all entries in TEXTLIST, one QUALTEXT per line

EOF
}

function fail {
    echo "==="
    echo $0: $1. Stop.
    echo "==="
    usage
    exit 1
}

function texts {
    if [ ! -r $1 ]; then
	fail "No such text list $1"
    fi
    echo $0 checking contents of $1
    while read l
    do
	t=`echo $l | cut -d@ -f1`
	>2 echo ::$t
	cfy -U$unilist -q $t
    done <$1
}

if [[ "$*" == "" ]]; then
    usage
    exit 0
fi

while [[ $# -gt 0 ]]; do
    case $1 in
	-p)
	    check=p
	    arg="$2"
	    [ -d ${ORACC}/$arg ] || fail "Unknown project $arg"
	    shift
	    shift
	    ;;
	-q)
	    check=q
	    arg="$2"
	    shift
	    shift
	    ;;
	-t)
	    check=t
	    arg="$2"
	    shift
	    shift
	    ;;
	-u)
	    unilist="$2"
	    shift
	    shift
	    ;;
	-*)
	    echo "Unknown option $1"
	    usage
	    exit 1
	    ;;
	*)
	    shift
	    ;;
  esac
done

if [[ "$check" == "" ]]; then
    fail "No action requested--need -p, -q, or -t"
fi

if [[ "$unilist" == "" ]]; then
    fail "Must give -u UNILIST"
fi

if [[ "$arg" == "" ]]; then
    fail "No argument given with action"
fi

case $check in
    p)
	texts ${ORACC}/$arg/01bld/lists/txtindex.lst
    ;;
    q)
	cfy -U$unilist -q $arg
    ;;
    t)
	texts $arg
    ;;
esac
