<?xml version='1.0' encoding="utf-8"?>

<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:o="http://oracc.org/ns/ood/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="o">

<xsl:output method="xml" indent="yes" omit-xml-declaration="yes"/>

<xsl:param name="oracc" select="'/Users/stinney/orc'"/>
<xsl:param name="project" select="''"/>

<xsl:template match="/">
  <xsl:if test="string-length($project)=0">
    <xsl:message terminate="yes">p3-xmd-div.xsl: project not set. Stop.</xsl:message>
  </xsl:if>
  <xsl:apply-templates select="*/o:record"/>
</xsl:template>

<xsl:template match="o:record">
  <div class="ood">
      <h1><xsl:value-of select="@xml:id"/>: <xsl:value-of select="o:n"/></h1>
      <div columns="2">
	<xsl:for-each select="*">
	  <xsl:apply-templates select="."/>
	</xsl:for-each>
      </div>
  </div>
</xsl:template>

<xsl:template match="*">
  <p>
    <span class="ood-field"><xsl:value-of select="local-name(.)"/>:</span>
    <span class="ood-data"><xsl:copy-of select="."/></span>
  </p>
</xsl:template>

</xsl:stylesheet>
