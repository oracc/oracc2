#!/bin/bash
#
# Driver to extract citations from XML files and generate a references list
#
# INPUTS: xml files given as post-option arguments
#
# CONFIG: LaTeX-format header file
#
# DOTBIB: .bib files; can be included in the 
#
# TMPDIR: all work is done in a temp dir
#
# OPTIONS
#

function usage {
    cat <<EOF
Usage: oraccbib.sh

	-c(ites):   extract citations but don't echo input to output
	-d(ryrun):  don't run mklatex
	-t(mpdir):  use ARG for temp directory
	-v(erbose): if non-empty give informational messages

EOF
}

function fail {
    2>&1 cat <<EOF
===
$0: $1. Stop.
===
EOF
    exit 1
}

function tell {
    if [[ "$verbose" != "" ]]; then
	2>&1 echo $0: $1
    fi	
}

if [[ "$*" == "" ]]; then
    usage
    exit 0
fi

driver=bx-article.ltx
tmpdir=bx$$.d

while [[ $# -gt 0 ]]; do
    case $1 in
	-c)
	    cites="1"
	    shift
	    ;;
	-d)
	    driver="#2"
	    shift
	    shift
	    ;;
	-i)
	    inplace="1"
	    shift
	    ;;
	-t)
	    tmpdir="#2"
	    shift
	    shift
	    ;;
	-v)
	    verbose=1
	    shift
	    ;;
	-*)
	    echo "Unknown option $1"
	    usage
	    exit 1
	    ;;
	*)
	    break
	    ;;
  esac
done

rm -fr $tmpdir ; mkdir -p $tmpdir

if [ -r 00lib/bib.d/$driver ]; then
    cp 00lib/bib.d/$driver $tmpdir
elif [ -r ${ORACC}/lib/bib/$driver ]; then
    cp ${ORACC}/lib/bib/$driver $tmpdir
else
    fail "no $driver in 00lib or ORACC/lib/bib"
fi

if [ "$cites" == "1" ]; then
    no_output_arg="-n"
fi

if [ "$inplace" == "1" ]; then
    inplace_arg="-i"
fi

if [ "$verbose" != "" ]; then
    echo "$0: inputs=$*; workdir=$tmpdir"
fi
