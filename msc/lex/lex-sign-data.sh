#!/bin/sh
projects=$*
libscripts=$ORACC_BUILDS/lib/scripts
if [ "$projects" == "" ]; then
    echo 'lex-sign-data.sh: must give project(s) on command line'
    exit 1
fi
rm -fr 01tmp/lex/sign
mkdir -p 01tmp/lex/sign
for p in $* ; do
    withlem=$ORACC_BUILDS/$p/01bld/lists/withlem
    for a in `pqxpand xtf <$withlem | xargs grep -l -F 'subtype="sg"'`; do
       	lex-sign-xtf.sh $a
    done
done
lex-master.sh sign
(cd 01tmp/lex ;
 xsltproc --xinclude $libscripts/lex-sign-group.xsl sign-master.xml >lex-sign-grouped.xml)
xsltproc $libscripts/lex-refs-group.xsl 01tmp/lex/lex-sign-grouped.xml >01tmp/lex/lex-sign-refs.xml
xsltproc $libscripts/lex-sign-sortkeys.xsl 01tmp/lex/lex-sign-refs.xml | sort -u | grep -v '^$' >01tmp/lex/sortkeys.txt
gdlx -g <01tmp/lex/sortkeys.txt | cat -n | sed 's/^ \+//' | rocox -R '<t c="%1">%2</t>' -x sort >01tmp/lex/sortcodes.xml
xsltproc $libscripts/lex-sign-ref-codes.xsl 01tmp/lex/lex-sign-refs.xml >01tmp/lex/lex-sign-refs-coded.xml
xsltproc $libscripts/lex-sign-ref-sort.xsl 01tmp/lex/lex-sign-refs-coded.xml >01tmp/lex/lex-sign-refs-sorted.xml
lex-collapse-refs 01tmp/lex/lex-sign-refs-sorted.xml >01tmp/lex/lex-sign-refs-collapsed.xml
xsltproc $libscripts/lex-sign-tis.xsl 01tmp/lex/lex-sign-refs-collapsed.xml >02pub/sign.tis
xsltproc $libscripts/lex-sign-HTML.xsl 01tmp/lex/lex-sign-refs-collapsed.xml >02pub/lex-sign.xhtml
