<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:xh="http://www.w3.org/1999/xhtml"
	       version="1.0">

<xsl:output method="text"/>
  
<xsl:template match="/">
  <xsl:variable name="zoom-ok">
    <xsl:call-template name="has-zoom"/>
  </xsl:variable>
  <xsl:variable name="sum-ok">
    <xsl:call-template name="has-sum"/>
  </xsl:variable>
  <xsl:if test="string-length($zoom-ok)>0 and string-length($sum-ok)>0">
    <xsl:text>ok</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template name="has-zoom">
  <xsl:for-each select=".//xh:div[@id='p4Letters']">
    <xsl:if test="count(xh:p)>0">
      <xsl:text>ok</xsl:text>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

<xsl:template name="has-sum">
  <xsl:for-each select=".//xh:div[@class='summ']">
    <xsl:if test="count(xh:p)>0">
      <xsl:text>ok</xsl:text>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

</xsl:transform>
