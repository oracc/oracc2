<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:struct="http://oracc.org/ns/esp-struct/1.0"
    xmlns:xi="http://www.w3.org/2001/XInclude"
    exclude-result-prefixes="sl dc xh"
    version="1.0">

<xsl:output method="xml" indent="yes" encoding="utf-8"/>

<xsl:template match="/">
  <struct:page file="index.xml" id="home" type="page" target="_top">
    <xi:include xml:base="." href="00lib/signlist-structure-top.xml" xpointer="xpointer(/*/*)">
      <xi:fallback>
	<xi:include href="/home/oracc/lib/data/signlist-structure-top.xml" xpointer="xpointer(/*/*)">
	  <xi:fallback/>
	</xi:include>
      </xi:fallback>
    </xi:include>
    <hr/>
    <xsl:if test="not(/*/@signlist='ogsl')">
      <struct:page file="overview.xml" id="overview" type="page"/>
    </xsl:if>
    <xsl:for-each select="*/sl:letter[not(@num='1')]">
      <struct:page file="{@xml:id}.xml" id="{@xml:id}" type="page">
	<xsl:for-each select="sl:sign[not(@moid)]">
	  <struct:page file="{@xml:id}.xml" id="{@xml:id}" type="page" hide-menu-link="yes"/>
	</xsl:for-each>
      </struct:page>
    </xsl:for-each>
    <struct:page file="numbers.xml" id="numbers" type="page">
      <xsl:for-each select="*/sl:letter[@num='1']">
	<struct:page file="{@xml:id}.xml" id="{@xml:id}" type="page" hide-menu-link="yes">
	  <xsl:for-each select="sl:sign[not(@moid)]">
	    <struct:page file="{@xml:id}.xml" id="{@xml:id}" type="page" hide-menu-link="yes"
			 />
<!--			 show-small-print-link="yes" purl="/{/*/@project}/{@xml:id}" -->
	  </xsl:for-each>
	</struct:page>
    </xsl:for-each>
    </struct:page>
    <hr/>
    <xi:include xml:base="." href="00lib/signlist-structure-bot.xml" xpointer="xpointer(/*/*)">
      <xi:fallback>
	<xi:include href="/home/oracc/lib/data/signlist-structure-bot.xml" xpointer="xpointer(/*/*)">
	  <xi:fallback/>
	</xi:include>
      </xi:fallback>
    </xi:include>
  </struct:page>
</xsl:template>

</xsl:transform>
