<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:sxw="http://oracc.org/ns/sxweb/1.0"
	       version="1.0"
	       >

  <xsl:template match="sxw:page-o-signs">
    <xsl:message><xsl:value-of select="local-name(.)"/></xsl:message>
    <xsl:variable name="ids" select="text()"/>
    <xsl:for-each select="document('/home/oracc/osl/02xml/sl.xml')">
      <xsl:message><xsl:value-of select="/*/@signlist"/></xsl:message>
      <xsl:message>ids="<xsl:value-of select="$ids"/>"</xsl:message>
      <xsl:for-each select="id($ids)">
	<xsl:sort data-type="number" select="@sort"/>
	<xsl:message><xsl:value-of select="@n"/></xsl:message>
      </xsl:for-each>
    </xsl:for-each>
  </xsl:template>
  
  <xsl:template match="*">
    <xsl:message><xsl:value-of select="local-name(.)"/></xsl:message>
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

</xsl:transform>
