<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:xh="http://www.w3.org/1999/xhtml"
	       version="1.0">

<xsl:output method="text"/>
  
<xsl:template match="/">
  <xsl:variable name="art-ok">
    <xsl:call-template name="has-art"/>
  </xsl:variable>
  <xsl:if test="string-length($art-ok)>0">
    <xsl:text>ok</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template name="has-art">
  <xsl:for-each select=".//xh:div[@id='p4GlossaryEntry']">
    <xsl:if test="count(.//xh:span[@class='gw'])>0">
      <xsl:text>ok</xsl:text>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

</xsl:transform>
