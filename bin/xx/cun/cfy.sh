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
</html>
EOF
}

function logfail {
    if [[ "$mode" == "web" ]]; then
	logfailw $*
    else
	echo "==="
	echo $0: $1 failed, log file follows. Stop
	echo "==="
	cat $2
	exit 1	
    fi
}

function logfailw {
    cat <<EOF
Content-type: text/html; charset=utf-8

<html>
<head><title>Cuneify Error</title></head>
<body>
<h1>$1 failed</h1>
<h2>Error log:</h2>
EOF
    xmlify <$2
    cat <<EOF
</body>
</html>
EOF
    exit 1	
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
	-r|-rem)
	    rmfile=1
	    shift
	    ;;
	-T|-TeX)
	    mode=TeX
	    shift
	    ;;
	-t|-text)
	    mode=text
	    shift
	    ;;
	-v|-verbose)
	    verbose=1
	    shift
	    ;;
	-w|-web)
	    mode=web
	    shift
	    ;;
	-x|-xml)
	    mode=xml
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

echo atf=$atf

if [[ "$atf" != "" ]]; then
    if [[ "$file" != "" ]]; then
	fail "can't use both -atf and -file"
    fi
    file="/tmp/$$.cfy.tmp"
    echo "$atf" >$file
    if [ $? -ne 0 ]; then
	fail "unable to write $file for ATF text"
    fi
    file=$file
    tell "ATF string arg written to $file"
fi

if [[ "$file" == "" ]]; then
    fail "must give give -atf TEXT or -file FILE"
fi

atffile="/tmp/$$.cfy.atf"
cfyfile="/tmp/$$.cfy.cfy"
logfile="/tmp/$$.cfy.log"
xtffile="/tmp/$$.cfy.xtf"

# Create .xtf with ATF header if necessary
atfheader.sh $file $lang >$atffile
oxx -l$logfile $atffile >$xtffile

if [ $? -ne 0 ]; then
    logfail 'ATF processing' $logfile
fi

# Could create LGS table here; is it worth it for cuneify.html?

cfy -w $xtffile >$cfyfile 2>$logfile
if [ $? -ne 0 ]; then
    logfail 'Cuneify processing' $logfile
else
    cat $cfyfile
fi

rm -f $atffile $xtffile $cfyfile $logfile
if [[ "$rmfile" == "" ]]; then
    rm -f $file
fi

exit 0
