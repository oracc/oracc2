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

function usage {
    cat <<EOF
Usage: oraccbib.sh

	-verbose: if non-empty give informational messages

EOF
}

function fail {
    2>&1 cat <<EOF
    "==="
    $0: $1. Stop.
    "==="
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

