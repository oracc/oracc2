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

<xsl:output method="text" indent="yes" encoding="utf-8"/>

<xsl:template match="/">
  <xsl:for-each select="*/sl:letter[not(@num='1')]">
    <xsl:call-template name="oids"/>
  </xsl:for-each>
  <xsl:for-each select="*/sl:letter[@num='1']">
    <xsl:call-template name="oids">
      <xsl:with-param name="n" select="'/Numbers'"/>
    </xsl:call-template>
  </xsl:for-each>
</xsl:template>

<xsl:template name="oids">
  <xsl:param name="n" select="''"/>
  <xsl:for-each select=".//sl:sign">
    <xsl:value-of select="concat(
			  @xml:id, '&#x9;u1&#x9;',
			  '/',/*/@project, '/signlist',$n,
			  '/',ancestor::sl:letter/@xml:id,
			  '/',@xml:id,
			  '&#xa;')"/>
    <xsl:for-each select=".//sl:form">
      <xsl:if test="starts-with(@oid,'o')">
	<xsl:value-of select="concat(
			      @oid, '&#x9;u2&#x9;',
			      '/',/*/@project, '/signlist',$n,
			      '/',ancestor::sl:letter/@xml:id,
			      '/',ancestor::sl:sign/@xml:id,
			      '&#xa;')"/>
      </xsl:if>
    </xsl:for-each>
  </xsl:for-each>
</xsl:template>

</xsl:transform>
