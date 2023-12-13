<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:xi="http://www.w3.org/2001/XInclude"
    xmlns:c="http://oracc.org/ns/xcf/1.0"
    version="1.0">

<xsl:param name="xcfname"/>
<xsl:output method="text" indent="yes" encoding="utf-8"/>

<xsl:template match="c:set[@var='process']">
  <!--<xsl:message>k=<xsl:value-of select="c:key[@k='action']/@to"/></xsl:message>-->
  <xsl:variable name="input">
    <xsl:choose>
      <xsl:when test="c:key[@k='input']">
	<xsl:value-of select="c:key[@k='input']/@to"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:message>preceding k=input=<xsl:value-of select="count(preceding-sibling::*[c:key[@k='input']])"/></xsl:message>
	<xsl:value-of select="preceding-sibling::*[c:key[@k='input']][1]/c:key[@k='input']/@to"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="output">
    <xsl:choose>
      <xsl:when test="c:key[@k='output']">
	<xsl:value-of select="c:key[@k='output']/@to"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="concat('01tmp/$xcfname-',count(preceding-sibling::*),'.xml')"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="action" select="c:key[@k='action']/@to"/>
  <xsl:variable name="actext" select="substring($action, string-length($action)-3)"/>
  <xsl:choose>
    <xsl:when test="$actext = '.xsl'">
      <xsl:text>xsltproc $xcfxslargs </xsl:text>
      <xsl:value-of select="concat('-o ', $output, ' ')"/>
      <xsl:value-of select="$action"/>
      <xsl:text> </xsl:text>
      <xsl:value-of select="$input"/>
      <xsl:text>&#xa;</xsl:text>
    </xsl:when>
    <xsl:when test="$actext = '.plx'">
      <xsl:value-of select="$action"/>
      <xsl:value-of select="concat(' -I',$input)"/>
      <xsl:text>&#xa;</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>xcfcmd </xsl:text>
      <xsl:value-of select="$action"/>
      <xsl:value-of select="concat(' -I',$input)"/>
      <xsl:text>&#xa;</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="text()"/>

</xsl:transform>
