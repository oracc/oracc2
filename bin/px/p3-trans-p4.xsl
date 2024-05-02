<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:output method="xml" indent="yes" encoding="UTF-8"/>
  
<xsl:template match="*|text()">
  <xsl:copy>
    <xsl:copy-of select="@*[not(name()='onchange')]"/>
    <xsl:if test="@onchange">
      <xsl:attribute name="onchange"><xsl:text>act_translation()</xsl:text></xsl:attribute>
    </xsl:if>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:transform>
