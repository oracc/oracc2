<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns:struct="http://oracc.org/ns/esp-struct/1.0"
    xmlns:xi="http://www.w3.org/2001/XInclude"
    exclude-result-prefixes="dc xh"
    version="1.0">

<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:template match="/">
  <sl:freq-tab slbase="{/*/@project}">
    <xsl:apply-templates select=".//sl:sign[@icnt>0]">
      <xsl:sort select="@ctotal" data-type="number" order="descending"/>
    </xsl:apply-templates>
  </sl:freq-tab>
</xsl:template>

<xsl:template match="sl:sign">
  <xsl:copy>
    <xsl:copy-of select="@n|@icnt|@xml:id"/>
    <xsl:choose>
      <xsl:when test="sl:cpds[@icnt > 0]">
	<xsl:attribute name="ccnt"><xsl:value-of select="sl:cpds/@icnt"/></xsl:attribute>
	<xsl:attribute name="tcnt"><xsl:value-of select="sl:cpds/@icnt+@icnt"/></xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
	<xsl:attribute name="tcnt"><xsl:value-of select="@icnt"/></xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:copy-of select="sl:ucun"/>
    <xsl:apply-templates select="sl:v[@icnt>0]">
      <xsl:sort select="@icnt" data-type="number" order="descending"/>
    </xsl:apply-templates>
    <xsl:apply-templates select="sl:form[@icnt>0]">
      <xsl:sort select="@icnt" data-type="number" order="descending"/>
    </xsl:apply-templates>
  </xsl:copy>
</xsl:template>

<xsl:template match="sl:form">
  <xsl:copy>
    <xsl:copy-of select="@n|@icnt|@oid"/>
    <xsl:apply-templates select="sl:v[@icnt>0]">
      <xsl:sort select="@icnt" data-type="number" order="descending"/>
    </xsl:apply-templates>
  </xsl:copy>
</xsl:template>

<xsl:template match="sl:v">
  <xsl:copy>
    <xsl:copy-of select="@n|@icnt"/>
  </xsl:copy>
</xsl:template>

</xsl:transform>
