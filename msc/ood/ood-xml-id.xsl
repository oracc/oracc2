<?xml version="1.0" encoding="utf-8"?>
<xsl:transform
    version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:o="http://oracc.org/ns/ood/1.0">
<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:if test="self::o:record">
      <xsl:attribute name="xml:id">
	<xsl:value-of select="concat('o',o:id)"/>
      </xsl:attribute>
    </xsl:if>
    <xsl:apply-templates select="*|text()"/>
  </xsl:copy>
</xsl:template>
</xsl:transform>
