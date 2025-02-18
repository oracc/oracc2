<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:sl="http://oracc.org/ns/sl/1.0"
	       version="1.0">

<xsl:output method="text" encoding="utf8"/>
  
<xsl:template match="sl:sign|sl:form|sl:pcun|sl:xsux">
  <xsl:value-of select="concat(@xml:id, '&#x9;', ./sl:ucun, '&#xa;')"/>
  <xsl:if test="@oid">
    <xsl:value-of select="concat(@oid, '&#x9;', ./sl:ucun, '&#xa;')"/>    
  </xsl:if>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="text()"/>

</xsl:transform>
