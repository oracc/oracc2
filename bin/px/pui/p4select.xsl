<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:output method="xml" indent="yes" encoding="UTF-8" omit-xml-declaration="yes"/>
  
<xsl:param name="select"/>

<xsl:template match="select">
  <xsl:copy>
    <xsl:copy-of select="@*[not(name()='value')]"/>
    <xsl:attribute name="value"><xsl:value-of select="$select"/></xsl:attribute>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="*|text()">
  <xsl:copy>
    <xsl:copy-of select="@*[not(name()='selected')]"/>
    <xsl:if test="@value=$select">
      <xsl:attribute name="selected"><xsl:text>selected</xsl:text></xsl:attribute>
    </xsl:if>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:transform>
