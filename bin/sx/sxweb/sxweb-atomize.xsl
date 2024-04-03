<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:sl="http://oracc.org/ns/sl/1.0"
	       xmlns:g="http://oracc.org/ns/gdl/1.0"
	       xmlns:ex="http://exslt.org/common"
	       extension-element-prefixes="ex"
	       version="1.0">

<xsl:output method="text" encoding="utf-8"/>
  
<xsl:template match="sl:sign|sl:form">
  <xsl:if test="not(sl:uage='0') and sl:sys[@name='AP23'][not(@token='not')]">
    <xsl:if test="starts-with(@n,'|')">
      <xsl:value-of select="@n"/>
      <xsl:text>&#x9;</xsl:text>
      <xsl:for-each select="sl:name/g:w/g:c/*[not(local-name()='o')]">
	<xsl:apply-templates mode="atom" select="."/>
	<xsl:if test="not(position()=last())"><xsl:text>&#x9;</xsl:text></xsl:if>
      </xsl:for-each>
      <xsl:text>&#xa;</xsl:text>
    </xsl:if>
  </xsl:if>
  <xsl:apply-templates select="sl:form"/>
</xsl:template>

<xsl:template mode="atom" match="g:o"/>

<xsl:template mode="atom" match="g:b">
  <xsl:value-of select="."/>
</xsl:template>

<xsl:template mode="atom" match="g:a">
  <xsl:if test="preceding-sibling::g:b">
    <xsl:text>~</xsl:text>
    <xsl:value-of select="."/>
  </xsl:if>
</xsl:template>

<xsl:template mode="atom" match="g:m">
  <xsl:text>@</xsl:text>
  <xsl:value-of select="."/>
</xsl:template>

<xsl:template mode="atom" match="g:n">
  <xsl:value-of select="@form"/>
</xsl:template>

<xsl:template mode="atom" match="g:s">
  <xsl:choose>
    <xsl:when test="g:b">
      <xsl:choose>
	<xsl:when test="g:m">
	  <xsl:for-each select="g:m">
	    <xsl:for-each select="preceding-sibling::*|.">
	      <xsl:apply-templates mode="atom" select="."/>
	    </xsl:for-each>
	    <xsl:if test="not(position()=last())">
	      <xsl:text>&#x9;</xsl:text>
	    </xsl:if>
	  </xsl:for-each>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:apply-templates mode="atom"/>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="."/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template mode="atom" match="g:g">
  <xsl:apply-templates mode="group"/>
</xsl:template>

<xsl:template mode="group" match="g:o">
  <xsl:choose>
    <xsl:when test="@g:type='above'"><xsl:text>&amp;</xsl:text></xsl:when>
    <xsl:when test="@g:type='beside'"><xsl:text>.</xsl:text></xsl:when>
    <xsl:when test="@g:type='containing'"><xsl:text>×</xsl:text></xsl:when>
    <xsl:when test="@g:type='joining'"><xsl:text>+</xsl:text></xsl:when>
    <xsl:otherwise>
      <xsl:message>no render for g:o <xsl:value-of select="g:type"/></xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template mode="group" match="g:b">
  <xsl:value-of select="."/>
</xsl:template>

<xsl:template mode="group" match="g:a">
  <xsl:text>~</xsl:text>
  <xsl:value-of select="."/>
</xsl:template>

<xsl:template mode="group" match="g:m">
  <xsl:text>@</xsl:text>
  <xsl:value-of select="."/>
</xsl:template>

<xsl:template mode="group" match="g:n">
  <xsl:value-of select="@form"/>
</xsl:template>

<xsl:template mode="group" match="g:s">
  <xsl:choose>
    <xsl:when test="g:b">
      <xsl:apply-templates mode="group"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="."/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="text()"/>

</xsl:transform>
