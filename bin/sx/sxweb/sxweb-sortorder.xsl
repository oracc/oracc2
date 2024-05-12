<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:esp="http://oracc.org/ns/esp/1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:xh="http://www.w3.org/1999/xhtml"    
    version="1.0">

<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:template match="x">
  <esp:page>
    <esp:name>Sort Order</esp:name>
    <esp:title><xsl:value-of select="$project"/> Sort Order</esp:title>
    <html>
      <head/>
      <body>
	<p>The signs on this page are sorted according to the Oracc
	graphemic sort order as documented at <esp:link
	url="/osl/Sorting/">the OSL Sorting page</esp:link>.</p>
	<table class="pretty">
	  <xsl:apply-templates/>
	</table>
      </body>
    </html>
  </esp:page>
</xsl:template>

<xsl:template match="r">
  <tr><xsl:apply-templates/></tr>
</xsl:template>

<xsl:template match="c">
  <xsl:variable name="oid" select="substring-before(text(),'_')"/>
  <xsl:variable name="sign" select="substring-after(text(),'_')"/>
  <td><esp:link url="/{$project}/signlist/{$oid}"><xsl:value-of select="$sign"/></esp:link></td>
</xsl:template>

</xsl:transform>
