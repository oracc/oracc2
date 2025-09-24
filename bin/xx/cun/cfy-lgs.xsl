<xsl:transform 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xtf="http://oracc.org/ns/xtf/1.0"
  xmlns:gdl="http://oracc.org/ns/gdl/1.0"
  version="1.0" >

  <xsl:output method="text" encoding="UTF-8"/>
  
  <xsl:template match="xtf:l[@type='lgs']">
    <xsl:for-each select=".//gdl:s">
      <xsl:variable name="u" select="@gdl:utf8"/>
      <xsl:variable
	  name="r"
	  select="ancestor::xtf:lg/xtf:l[1]/gdl:w[contains(.//@gdl:utf8, $u)]"/>
      <xsl:value-of select="@xml:id"/>
      <xsl:text>&#x9;</xsl:text>
      <xsl:for-each select="$r">
	<xsl:value-of select="@xml:id"/>
	<xsl:if test="not(position()=last())"><xsl:text> </xsl:text></xsl:if>
      </xsl:for-each>
      <xsl:text>&#xa;</xsl:text>
    </xsl:for-each>
  </xsl:template>

  <xsl:template match="text()"/>
  
</xsl:transform>
