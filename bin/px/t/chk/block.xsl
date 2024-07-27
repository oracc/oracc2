<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:xh="http://www.w3.org/1999/xhtml"
	       version="1.0">

<xsl:output method="text"/>
  
<xsl:template match="/">
  <xsl:for-each select=".//xh:table[@class='score_block']">
    <xsl:text>ok</xsl:text>
  </xsl:for-each>
</xsl:template>

</xsl:transform>
