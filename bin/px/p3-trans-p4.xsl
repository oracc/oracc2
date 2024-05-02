<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:output method="xml" indent="yes" encoding="UTF-8"/>
  
<xsl:template match="*|text()">
  <xsl:copy>
    <xsl:if test="@id">
      <xsl:attribute name="id"><xsl:text>p4TransSelect</xsl:text></xsl:attribute>
    </xsl:if>
    <xsl:if test="@onchange">
      <xsl:attribute name="onchange"><xsl:text>act_translation()</xsl:text></xsl:attribute>
    </xsl:if>
    <xsl:copy-of select="@value|@selected"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:transform>
