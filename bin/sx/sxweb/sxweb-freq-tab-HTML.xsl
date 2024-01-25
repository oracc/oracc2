<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns:esp="http://oracc.org/ns/esp/1.0"
    xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:struct="http://oracc.org/ns/esp-struct/1.0"
    xmlns:xi="http://www.w3.org/2001/XInclude"
    exclude-result-prefixes="dc xh"
    version="1.0">

<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:template match="/">
  <table class="pretty">
    <xsl:apply-templates/>
  </table>
</xsl:template>

<xsl:template match="sl:sign">
  <tr>
    <td><esp:link url="/{/*/@slbase}/{@xml:id}"><xsl:value-of select="@n"/></esp:link></td>
    <td><xsl:value-of select="@tcnt"/></td>
  </tr>
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
