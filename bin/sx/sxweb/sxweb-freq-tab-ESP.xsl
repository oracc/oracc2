<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns:esp="http://oracc.org/ns/esp/1.0"
    xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:struct="http://oracc.org/ns/esp-struct/1.0"
    xmlns:xi="http://www.w3.org/2001/XInclude"
    exclude-result-prefixes="dc xh"
    version="1.0">

<xsl:param name="project"/>
<xsl:include href="sxweb-util.xsl"/>

<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:template match="/">
  <esp:page
      xmlns:esp="http://oracc.org/ns/esp/1.0"
      xmlns="http://www.w3.org/1999/xhtml"
      xmlns:xh="http://www.w3.org/1999/xhtml">
    <esp:name>Frequencies</esp:name>
    <esp:title>Sign Frequencies</esp:title>
    <html>
      <head/>
      <body>
	<table class="pretty">
	  <xsl:apply-templates/>
	</table>
      </body>
    </html>
  </esp:page>
</xsl:template>

<xsl:template match="sl:sign">
  <tr>
    <xsl:if test="not(position() mod 10)">
      <xsl:attribute name="class"><xsl:text>tithe</xsl:text></xsl:attribute>
    </xsl:if>
    <td><xsl:value-of select="position()"/><xsl:text>.</xsl:text></td>
    <td><p class="{$font}"><xsl:call-template name="sxweb-ucun"/></p></td>
    <td><esp:link url="/{/*/@slbase}/signlist/{@xml:id}"><xsl:value-of select="@n"/></esp:link></td>
    <td><xsl:value-of select="@tcnt"/></td>
    <td>
      <xsl:for-each select="sl:v">
	<p>
	  <xsl:value-of select="@n"/>
	  <xsl:text> (</xsl:text><xsl:value-of select="@icnt"/>
	  <xsl:text>×</xsl:text>
	  <xsl:variable name="alemma" select="sl:lemmas/sl:lemma[@gpos='a']"/>
	  <xsl:variable name="xlemma" select="sl:lemmas/sl:lemma[not(@gpos='a')]"/>
	  <xsl:if test="count($alemma)>0">
	    <xsl:text> in </xsl:text>
	    <xsl:for-each select="$alemma">
	      <xsl:value-of select="@n"/>
	      <xsl:if test="not(@lcnt=../../@icnt)">
		<xsl:text> (</xsl:text>
		<xsl:value-of select="@lcnt"/>
		<xsl:text>×)</xsl:text>
	      </xsl:if>
	      <xsl:if test="not(position()=last())"><xsl:text>, </xsl:text></xsl:if>
	    </xsl:for-each>
	  </xsl:if>
	  <xsl:if test="count($xlemma)>0">
	    <esp:link url="/{/*/@slbase}/signlist/{ancestor::sl:sign/@xml:id}">
	      <xsl:text> See more</xsl:text>
	    </esp:link>
	  </xsl:if>
	  <xsl:text>)</xsl:text>
	  <xsl:if test="not(position()=last())"><xsl:text>; </xsl:text></xsl:if>
	</p>
      </xsl:for-each>
    </td>
  </tr>
</xsl:template>

<xsl:template match="sl:xform">
  <xsl:copy>
    <xsl:copy-of select="@n|@icnt|@oid"/>
    <xsl:apply-templates select="sl:v[@icnt>0]">
      <xsl:sort select="@icnt" data-type="number" order="descending"/>
    </xsl:apply-templates>
  </xsl:copy>
</xsl:template>

<xsl:template match="sl:xv">
  <xsl:copy>
    <xsl:copy-of select="@n|@icnt"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="sl:scripts"/>

</xsl:transform>
