<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:xh="http://www.w3.org/1999/xhtml"
	       version="1.0">

<xsl:output method="text"/>
  
<xsl:template match="/">
  <xsl:variable name="zoom-ok">
    <xsl:call-template name="has-zoom"/>
  </xsl:variable>
  <xsl:variable name="ref-ok">
    <xsl:call-template name="has-ref"/>
  </xsl:variable>
  <xsl:if test="string-length($zoom-ok)>0 and string-length($ref-ok)>0">
    <xsl:text>ok</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template name="has-zoom">
  <xsl:for-each select=".//xh:div[@id='p4MenuOutline']">
    <xsl:if test="count(xh:h2)>0">
      <xsl:text>ok</xsl:text>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

<xsl:template name="has-ref">
  <xsl:for-each select=".//xh:div[@id='p4Content']">
    <xsl:if test="count(.//xh:p[@class='refline tr'])>0">
      <xsl:text>ok</xsl:text>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

</xsl:transform>