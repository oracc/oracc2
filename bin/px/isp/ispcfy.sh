#!/bin/dash
#
# Create, e.g., /home/oracc/p4.d/htm/dcclt/P000/P000001/P000001.cfy
# which is the cuneified XTF data wrapped in a div.
#
# $1=prox-project $2=PQX $3=html-dir $4=cachedir
#
# Exit
#	0 on success
#	1 on error
#	2 not used (ispxtf.sh compatibility)
#	3 no XTF--transliteration not available
#

>&2 echo $0 $*

oraccbin=${ORACC}/bin
prox_project=$1
PQX=$2
htmldir=$3
cachedir=$4

period=`$oraccbin/xmdx $prox_project $PQX`
>&2 echo $0: period=$period
if [ "$period" != "" ]; then
    xperiod=`/bin/echo -n $period | tr ' ' '_'`
    Parg=" -P $xperiod"
    >&2 echo $0: Parg=$Parg
fi

if [ "$cachedir" != "" ]; then
    errlog=$cachedir/err.log
    rm -f $errlog
    >&2 echo $0: args=$*
    echo $0: transferring error logging to $errlog >&2
    exec 2>$errlog
    echo $0 $* >&2
else
    true
fi

#    exec 2>/dev/null

# Check the .xtf exists
xtf=`$oraccbin/pqxpand xtf $prox_project:$PQX`
[ -r $xtf ] || exit 3

# Check we have or can make the htmldir
>&2 mkdir -v -p $htmldir || exit 1

# Generate the cuneified output
>&2 echo $0 calling "$oraccbin/cunx $Parg -vw -q $prox_project:$PQX" 
$oraccbin/cunx $Parg -vw -q $prox_project:$PQX

if [ ! $? -eq 0 ]; then
    echo "Failed call invoked as:" >&2
    echo "	$oraccbin/cunx $Parg -w -q $prox_project:$PQX" >&2
    echo $0: cunx failed. Stop. >&2
    exit 1
fi
#
# Removing the errlog when exiting 0 is the right thing to do but it
# means that we have to be careful in the above to test $? as we go
# and exit 1 appropriately.
#
##if [ $? -eq 0 ]; then
##    rm -f $errlog
##fi

# exit with last program's status; don't exit 0/1 based on empty
# errlog because that could contain diagnostics
