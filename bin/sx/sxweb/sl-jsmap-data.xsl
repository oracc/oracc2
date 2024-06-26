<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:struct="http://oracc.org/ns/esp-struct/1.0"
    xmlns:xi="http://www.w3.org/2001/XInclude"
    exclude-result-prefixes="sl dc xh"
    version="1.0">

<xsl:output method="text" encoding="utf-8"/>
  
<xsl:variable name="u" select="'ʾāŋḫšṣṭABCDEFGŊHḪIJKLMNOPQRSŠṢTṬUVWXYZ₀₁₂₃₄₅₆₇₈₉ₓ×|'"/>
<xsl:variable name="a" select="'yajhcfvabcdefgjhhijklmnopqrscftvuvwxyz0123456789x*'"/>

<xsl:template name="out-n">
  <xsl:param name="n" select="@n"/>
  <xsl:choose>
    <xsl:when test="contains($n,'(')">
      <!--
      <xsl:variable name="b" select="substring-after($n,'(')"/>
      <xsl:call-template name="out-n">
	<xsl:with-param name="n" select="substring-before($b,')')"/>
      </xsl:call-template>
      -->
      <xsl:call-template name="out-n">
	<xsl:with-param name="n" select="translate($n,'()','')"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <!--<xsl:message><xsl:value-of select="$n"/> => <xsl:value-of select="translate($n,$u,$a)"/></xsl:message>-->
      <xsl:variable name="lid">
	<xsl:choose>
	  <xsl:when test="ancestor-or-self::sl:sign/@moid">
	    <xsl:for-each select="id(ancestor-or-self::sl:sign/@moid)">
	      <xsl:value-of select="../@xml:id"/>
	    </xsl:for-each>
	  </xsl:when>
	  <xsl:otherwise><xsl:value-of select="ancestor-or-self::sl:letter/@xml:id"/></xsl:otherwise>
	</xsl:choose>
      </xsl:variable>
      <xsl:variable name="sid">
	<xsl:choose>
	  <xsl:when test="ancestor-or-self::sl:sign/@moid">
	    <xsl:value-of select="ancestor-or-self::sl:sign/@moid"/>
	  </xsl:when>
	  <xsl:otherwise><xsl:value-of select="ancestor-or-self::sl:sign/@xml:id"/></xsl:otherwise>
	</xsl:choose>
      </xsl:variable>
      <xsl:value-of select="concat(translate($n,$u,$a), '&#x9;',$lid,'/',$sid,'&#xa;')"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="cpd">
  <xsl:param name="n"/>
  <xsl:param name="nodes"/>
  <xsl:if test="count($nodes)>0">
    <xsl:value-of select="$n"/><xsl:text>&#x9;</xsl:text>
    <xsl:for-each select="$nodes">
      <xsl:if test="not(position()=1)"><xsl:text> </xsl:text></xsl:if>
      <xsl:value-of select="@xml:id"/>
    </xsl:for-each>
    <xsl:text>&#xa;</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="/">
  <xsl:apply-templates select=".//sl:sign"/>
  <xsl:apply-templates select=".//sl:form"/>
  <xsl:apply-templates select=".//sl:homophones/sl:base"/>
  <xsl:call-template name="cpd">
    <xsl:with-param name="n" select="'memb*'"/>
    <xsl:with-param name="nodes" select=".//sl:sign[sl:cpds]"/>
  </xsl:call-template>
  <xsl:call-template name="cpd">
    <xsl:with-param name="n" select="'ctnr*'"/>
    <xsl:with-param name="nodes" select=".//sl:sign[sl:cpds/sl:ctnr]"/>
  </xsl:call-template>
  <xsl:call-template name="cpd">
    <xsl:with-param name="n" select="'ctnd*'"/>
    <xsl:with-param name="nodes" select=".//sl:sign[sl:cpds/sl:ctnd]"/>
  </xsl:call-template>
</xsl:template>

<xsl:template match="sl:sign|sl:form|sl:list|sl:v">
  <xsl:call-template name="out-n"/>
  <xsl:apply-templates select="sl:list|sl:v"/>
</xsl:template>

<xsl:template match="sl:homophones/sl:base">
  <xsl:choose>
    <xsl:when test="@count='1'">
      <xsl:variable name="out-n-n" select="concat(@n,'0')"/>
      <xsl:for-each select="id(*/@oid)">
	<!--<xsl:message>sl:base/out-n n=<xsl:value-of select="@n"/></xsl:message>-->
	<xsl:call-template name="out-n">
	  <xsl:with-param name="n" select="$out-n-n"/>
	</xsl:call-template>
      </xsl:for-each>
    </xsl:when>
    <xsl:otherwise>      
      <xsl:value-of select="concat(translate(@n,$u,$a), '0&#x9;selpages/',@xml:id,'&#xa;')"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="text()"/>

</xsl:transform>
