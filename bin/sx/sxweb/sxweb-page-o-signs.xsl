<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:sxw="http://oracc.org/ns/1.0/sxweb"
	       version="1.0"
	       >

  <xsl:template match="sxw:page-o-signs">
    <xsl:message><xsl:value-of select="local-name(.)"/></xsl:message>
    <xsl:variable name="ids" select="text()"/>
    <xsl:for-each select="document('/home/oracc/osl/02xml/sl.xml')">
      <xsl:message><xsl:value-of select="/*/@signlist"/></xsl:message>
      <xsl:message>ids="<xsl:value-of select="$ids"/>"</xsl:message>
      <xsl:for-each select="id('o0223341')">
	<xsl:message><xsl:value-of select="@name"/></xsl:message>
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
