#!/bin/sh
#
# For use with odo as odo subproject [-inherit] subname subabbrev subfullname
#
# As usual, odo calls this script with 'user' and 'project' as first two args.
#
. ${ORACC}/bin/odo-func.sh
odo_odo $0 $*

libscripts="${ORACC}/lib/scripts"

user=$odo_user
parent=$odo_proj

if [ "$1" = "-inherit" ]; then
    inherit="1"
    shift
fi

subname=$1 ; shift

if [ "$1" = "" ]; then
    echo $0: must give project abbreviation as arg2. Stop.
    exit 1
fi

subabbrev=$1 ; shift

subfullname=$*

if [ "$subfullname" = "" ]; then
    echo $0: must give project abbreviation as args 3..n. Stop.
    exit 1
fi

xsubname=`/bin/echo -n $subname | tr -d a-zA-Z0-9`

if [ "$xsubname" != "" ]; then
    echo $0: bad characters in subname. Limited to a-zA-Z0-9. Stop.
    exit 1    
fi

if [ ! -d $subname ]; then
    mkdir -p $subname
fi

fullname=$parent/$subname

#
# Working in subproject dir from now on
#
cd $subname

umask 0026
for a in any atf bak bib bin cat lib map res tei web xml ; do \
    mkdir -p 00$a ; \
done
mkdir -p 00lib/lists 00res/downloads 00res/images 01tmp

cat <<EOF >00lib/config.xml
<project n="$fullname" 
    xmlns="http://oracc.org/ns/xpd/1.0"
    xmlns:xpd="http://oracc.org/ns/xpd/1.0">
  <name>$subfullname</name>
  <abbrev>$subabbrev</abbrev>
  <project-type>user</project-type>
  <type>corpus</type>
  <logo type="text">$subabbrev</logo>
EOF

if [ -r ../${subname}.blurb ]; then
    /bin/echo -n '<blurb>' >>00lib/config.xml
    cat ../${subname}.blurb >>00lib/config.xml
    /bin/echo '</blurb>' >>00lib/config.xml
fi

xsltproc ${libscripts}/copy-options.xsl ../00lib/config.xml >>00lib/config.xml

echo '</project>' >>00lib/config.xml

if [ "$inherit" != "" ]; then
    config-inherit.sh $parent
fi

esp2-create.sh
