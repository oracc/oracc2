#!/bin/dash
libscripts="${ORACC_BUILDS}/lib/scripts"
p=`oraccopt`
xsltproc -stringparam project $p $libscripts/sxweb-freq-tab.xsl 02xml/sl.xml >01tmp/freq-tab.xml
xsltproc -stringparam project $p $libscripts/sxweb-freq-tab-ESP.xsl 01tmp/freq-tab.xml \
	 >signlist/00web/signlist-x-freq-table.xml
