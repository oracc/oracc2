<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xi="http://www.w3.org/2001/XInclude"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="xi">

<xsl:key name="sort" match="t" use="text()"/>

<xsl:template match="/*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="lex:group[@type='read' or @type='spel']">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:attribute name="sort">
      <xsl:variable name="v" select="@value"/>
      <xsl:for-each select="document('sortcodes.xml',/)">
	<xsl:value-of select="key('sort', $v)/@c"/>
      </xsl:for-each>
    </xsl:attribute>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
