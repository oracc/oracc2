<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		version="1.0">
<xsl:output method="text" encoding="UTF-8"/>

<xsl:template match="lex:group[@type='sign']">
  <xsl:variable name="xis" select=".//lex:group[@type='refs'][string-length(@xis)>0]"/>
  <xsl:value-of select="concat('lx.',@oid,'&#x9;', count($xis), '&#x9;')"/>
  <xsl:for-each select="$xis">
    <xsl:value-of select="@xis"/>
    <xsl:if test="not(position() = last())"><xsl:text> </xsl:text></xsl:if>
  </xsl:for-each>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

</xsl:stylesheet>
