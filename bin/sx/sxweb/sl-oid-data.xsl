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
 
<xsl:template name="out-n">
      <xsl:value-of select="concat(@n, '&#x9;',@xml:id,'&#xa;')"/>
</xsl:template>

<xsl:template match="/">
  <xsl:apply-templates select=".//sl:sign"/>
  <xsl:apply-templates select=".//sl:form"/>
</xsl:template>

<xsl:template match="sl:sign|sl:form|sl:list">
  <xsl:call-template name="out-n"/>
</xsl:template>

<xsl:template match="text()"/>

</xsl:transform>
