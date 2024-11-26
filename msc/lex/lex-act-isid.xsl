<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns="http://www.w3.org/1999/xhtml"
		xmlns:esp="http://oracc.org/ns/esp/1.0"
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:o="http://oracc.org/ns/oracc/1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="lex-act-isid">
  <xsl:param name="caller" select="'unknown'"/>
  <xsl:param name="isid" select="@xis|../@xis|@oid"/>
  <xsl:choose>
    <xsl:when test="string-length($isid)=0">
      <xsl:message>isid: <xsl:value-of select="$caller"/>: empty isid</xsl:message>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message><xsl:value-of select="$caller"/>: isid=<xsl:value-of select="$isid"/></xsl:message>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:attribute name="href"><xsl:text>javascript://</xsl:text></xsl:attribute>
  <xsl:attribute name="onclick"><xsl:text>act_isid(event)</xsl:text></xsl:attribute>
  <!-- The project and language should be passed in from caller after
       being set from config.xml options like cbd-word-base and
       cbd-word-phra -->
  <xsl:attribute name="data-proj"><xsl:text>dcclt</xsl:text></xsl:attribute>
  <xsl:attribute name="data-lang"><xsl:value-of select="@lang|*[1]/@lang"/></xsl:attribute>
  <xsl:attribute name="data-isid"><xsl:value-of select="$isid"/></xsl:attribute>
</xsl:template>

<xsl:template name="lex-act-iref">
  <xsl:param name="caller" select="'unknown'"/>
  <xsl:param name="iref" select="@xis|../@xis|@oid"/>
  <xsl:choose>
    <xsl:when test="string-length($isid)=0">
      <xsl:message>isid: <xsl:value-of select="$caller"/>: empty isid</xsl:message>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message><xsl:value-of select="$caller"/>: isid=<xsl:value-of select="$isid"/></xsl:message>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:attribute name="href"><xsl:text>javascript://</xsl:text></xsl:attribute>
  <xsl:attribute name="onclick"><xsl:text>act_isid(event)</xsl:text></xsl:attribute>
  <!-- The project and language should be passed in from caller after
       being set from config.xml options like cbd-word-base and
       cbd-word-phra -->
  <xsl:attribute name="data-proj"><xsl:text>dcclt</xsl:text></xsl:attribute>
  <xsl:attribute name="data-lang"><xsl:value-of select="@lang|*[1]/@lang"/></xsl:attribute>
  <xsl:attribute name="data-isid"><xsl:value-of select="$isid"/></xsl:attribute>
</xsl:template>

</xsl:stylesheet>
