<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<xsl:strip-space elements="*"/>
<xsl:output method="xml" indent="no" encoding="UTF-8"/>
  
<xsl:template match="*|text()">
  <xsl:copy>
    <xsl:choose>
      <xsl:when test="@id='p3OSdefault'">
	<xsl:attribute name="id"><xsl:text>p4OSdefault</xsl:text></xsl:attribute>
	<xsl:attribute name="onchange"><xsl:text>act_sorter_default()</xsl:text></xsl:attribute>
      </xsl:when>
      <xsl:when test="@id='p3OSspecial'">
	<xsl:attribute name="id"><xsl:text>p4OSspecial</xsl:text></xsl:attribute>	
	<xsl:attribute name="onchange"><xsl:text>act_sorter_special()</xsl:text></xsl:attribute>
      </xsl:when>
      <xsl:otherwise/>
    </xsl:choose>
    <xsl:copy-of select="@value|@selected"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:transform>
