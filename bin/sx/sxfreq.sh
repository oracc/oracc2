#!/bin/dash
libscripts="${ORACC_BUILDS}/lib/scripts"
xsltproc $libscripts/sxweb-freq-tab.xsl 02xml/sl.xml >01tmp/freq-tab.xml
xsltproc $libscripts/sxweb-freq-tab-ESP.xsl 01tmp/freq-tab.xml \
	 >signlist/00web/signlist-x-freq-table.xml
