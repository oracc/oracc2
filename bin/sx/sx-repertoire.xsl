<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    exclude-result-prefixes="sl"
    version="1.0">

<xsl:output method="text" encoding="utf-8"/>

<xsl:template match="sl:sign|sl:form">
  <xsl:value-of select="concat(@xml:id,'&#x9;',@n,'&#x9;',sl:ucun/@hex,'&#x9;')"/>
  <xsl:for-each select="sl:v">
    <xsl:value-of select="@n"/>
    <xsl:if test="not(position()=last())"><xsl:text> </xsl:text></xsl:if>
  </xsl:for-each>
  <xsl:if test="@compoundonly='yes'">
    <xsl:text>&#x9;</xsl:text>
    <xsl:for-each select="id(@cpd-refs)">
      <xsl:value-of select="@n"/>
      <xsl:if test="not(position()=last())"><xsl:text> </xsl:text></xsl:if>      
    </xsl:for-each>
  </xsl:if>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

</xsl:transform>
