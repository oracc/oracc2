#/bin/bash
#
# Script to cuneify text fragments posted by cuneify.html.
#
# When called by the Oracc url-resolver, wx, the script is exec'd and
# the first argument is '-web'; in this mode the output is an HTTP
# header and a complete Cuneify HTML page which is sent to an iframe.
#
# In web mode an error page is generated on failure.
#
# A future version of this script may support CLI XML/TeX/plain text
# generation.
#

function usage {
    cat <<EOF
Usage: cfy.sh [-web] [-atf ATF] [-file DATAFILE] [-key KEY] [-verbose]

	-atf	: ATF text
	-file	: a file of ATF text
	-key 	: a cfy key
	-TeX	: TeX output (not yet implemented)
	-text	: text output (not yet implemented)
	-verbose: if non-empty give informational messages
	-web    : flag for HTML output
	-xml	: XML output (incompletely implemented)

All of the options can be abbreviated to their first letter.

EOF
}

function fail {
    if [[ "$mode" == "web" ]]; then
	failw $*
    else
	echo "==="
	echo $0: $1. Stop.
	echo "==="
#	usage
	exit 1
    fi	
}

function failw {
    htmlerr $*
}

function htmlerr {
    x=`echo $* | xmlify`
    cat <<EOF
Content-type: text/html; charset=utf-8

<html>
<head><title>Cuneify Error</title></head>
<body>
<h1>Cuneify Error</h1>
<pre>$x</pre>
</body>
EOF
    }

function tell {
    if [[ "$verbose" != "" ]]; then
	echo $0: $1
    fi	
}

if [[ "$*" == "" ]]; then
    usage
    exit 0
fi

# default lang; when no project is set lang=sux means project=epsd2
lang="sux"

while [[ $# -gt 0 ]]; do
    case $1 in
	-a|-atf)
	    atf="$2"
	    shift
	    shift
	    ;;
	-f|-file)
	    file="$2"
	    shift
	    shift
	    ;;
	-l|-lang)
	    lang="$2"
	    shift
	    shift
	    ;;	    
	-k|-key)
	    key="$2"
	    shift
	    shift
	    ;;
	-T|-TeX)
	    mode=TeX
	    shift
	    shift
	    ;;
	-t|-text)
	    mode=TeX
	    shift
	    shift
	    ;;
	-v|-verbose)
	    verbose=1
	    shift
	    shift
	    ;;
	-w|-web)
	    mode=web
	    shift
	    shift
	    ;;
	-x|-xml)
	    mode=xml
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

if [[ "$atf" != "" ]]; then
    if [[ "$file" != "" ]]; then
	fail "can't use both -atf and -file"
    fi
    atffile="/tmp/$$.cfy.atf"
    echo "$atf" >$atffile
    if [ $? -ne 0 ]; then
	fail "unable to write $atffile for ATF text"
    fi
    file=$atffile
    tell "using $atffile"
fi

if [[ "$file" == "" ]]; then
    fail "must give give -atf TEXT or -file FILE"
fi

logfile="/tmp$$.cfy.log"
xtffile="/tmp$$.cfy.xtf"
head -10 $file | atfheader.sh $attfile | oxx -l$logfile >$xtffile

# if [[ "$unilist" == "" ]]; then
#     fail "Must give -u UNILIST"
# fi

# if [[ "$arg" == "" ]]; then
#     fail "No argument given with action"
# fi

# case $check in
#     p)
# 	texts ${ORACC}/$arg/01bld/lists/txtindex.lst
#     ;;
#     q)
# 	cfy -U$unilist -q $arg
#     ;;
#     t)
# 	texts $arg
#     ;;
# esac

# function texts {
#     if [ ! -r $1 ]; then
# 	fail "No such text list $1"
#     fi
#     echo $0 checking contents of $1
#     while read l
#     do
# 	t=`echo $l | cut -d@ -f1`
# 	>&2 echo ::$t
# 	cfy -U$unilist -q $t
#     done <$1
# }

