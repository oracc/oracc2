<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    exclude-result-prefixes="sl"
    version="1.0">

<xsl:output method="text" encoding="utf-8"/>

<xsl:template name="has-icnt">
  <xsl:choose>
    <xsl:when test="@icnt">
      <xsl:choose>
	<xsl:when test="@icnt='0'"><xsl:text>f</xsl:text></xsl:when>
	<xsl:otherwise><xsl:text>t</xsl:text></xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <xsl:choose>
	<xsl:when test="id(@cpd-refs)[@icnt][not(@icnt='0')]">
	  <xsl:text>t</xsl:text>
	</xsl:when>
	<xsl:otherwise><xsl:text>f</xsl:text></xsl:otherwise>
      </xsl:choose>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="sl:sign|sl:form">
  <xsl:variable name="occurs"><xsl:call-template name="has-icnt"/></xsl:variable>
  <xsl:if test="$occurs='t'">
    <xsl:variable name="nmoid">
      <xsl:value-of select="@n"/>
      <xsl:if test="@moid"><xsl:value-of select="concat('=',@moid)"/></xsl:if>
    </xsl:variable>
    <xsl:value-of select="concat(@xml:id,'&#x9;',$nmoid,'&#x9;',sl:ucun/@hex,'&#x9;')"/>
    <xsl:for-each select="sl:v">
      <xsl:value-of select="@n"/>
      <xsl:if test="not(position()=last())"><xsl:text> </xsl:text></xsl:if>
    </xsl:for-each>
    <xsl:if test="@compoundonly='yes'">
      <xsl:text>&#x9;</xsl:text>
      <xsl:for-each select="id(@cpd-refs)">
	<xsl:variable name="this-occurs"><xsl:call-template name="has-icnt"/></xsl:variable>
	<xsl:if test="$this-occurs='t'">
	  <xsl:value-of select="@xml:id"/>
	  <xsl:if test="not(position()=last())"><xsl:text> </xsl:text></xsl:if>
	</xsl:if>
      </xsl:for-each>
      <xsl:text>&#x9;</xsl:text>
      <xsl:for-each select="id(@cpd-refs)">
	<xsl:variable name="this-occurs"><xsl:call-template name="has-icnt"/></xsl:variable>
	<xsl:if test="$this-occurs='t'">
	  <xsl:value-of select="@n"/>
	  <xsl:if test="not(position()=last())"><xsl:text> </xsl:text></xsl:if>
	</xsl:if>
      </xsl:for-each>
    </xsl:if>
    <xsl:text>&#xa;</xsl:text>
  </xsl:if>
</xsl:template>

</xsl:transform>
