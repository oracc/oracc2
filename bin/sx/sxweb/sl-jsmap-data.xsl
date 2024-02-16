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
  
<xsl:variable name="u" select="'ŋḫšṣṭABCDEFGŊHḪIJKLMNOPQRSŠṢTṬUVWXYZ₀₁₂₃₄₅₆₇₈₉ₓ×|'"/>
<xsl:variable name="a" select="'jhcfvabcdefgjhhijklmnopqrscftvuvwxyz0123456789x*'"/>

<xsl:template name="out-n">
  <xsl:param name="n" select="@n"/>
  <xsl:choose>
    <xsl:when test="contains($n,'(')">
      <xsl:variable name="b" select="substring-after($n,'(')"/>
      <xsl:call-template name="out-n">
	<xsl:with-param name="n" select="substring-before($b,')')"/>
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

<xsl:template match="sl:sign|sl:form|sl:list|sl:v">
  <xsl:if test="not(starts-with(@n,'|'))">
    <xsl:call-template name="out-n"/>
  </xsl:if>
  <xsl:apply-templates/>
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
      <xsl:value-of select="concat(@n, '0&#x9;selpages/',@xml:id,'&#xa;')"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="text()"/>

</xsl:transform>
