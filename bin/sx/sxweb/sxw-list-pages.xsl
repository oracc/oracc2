<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:esp="http://oracc.org/ns/esp/1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:ex="http://exslt.org/common"
    extension-element-prefixes="ex"
    version="1.0">

  <xsl:output method="text"/>

  <xsl:template match="sl:signlist">
    <xsl:variable name="sl" select="."/>
    <xsl:for-each select="document('list-fonts.xml',/)/fonts/r">
      <xsl:if test="not(c[2]='-')">
	<xsl:variable name="list" select="c[1]"/>
	<xsl:for-each select="$sl/sl:listdef[@name=$list]">
	  <xsl:call-template name="list-page"/>
	</xsl:for-each>
      </xsl:if>
    </xsl:for-each>
  </xsl:template>
  
  <xsl:template name="list-page">
    <ex:document href="00lib/signlist-x-list-{@name}.xml">
<esp:page
    xmlns:esp="http://oracc.org/ns/esp/1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns:xi="http://www.w3.org/2001/XInclude"
    xmlns:sxw="http://oracc.org/ns/sxweb/1.0"
    >
  <esp:name><xsl:value-of select="@name"/></esp:name>
  <esp:title>List page for <xsl:value-of select="@name"/></esp:title>
  <html>
    <head/>
    <body class="sxw-list-page">
	<p>This page is derived from the <xsl:value-of select="/*/@project"/> sign list's coverage of:</p>

	<dl>
	  <dt><xsl:value-of select="@name"/></dt>
	  <dd><xsl:value-of select="sl:lit"/></dd>
	  <dt class="dt-sub">Numbers:</dt>
	  <dd><xsl:value-of select="sl:info"/></dd>
	  <xsl:if test="sl:note">
	    <dt>class="dt-sub">Notes:</dt>
	    <dd><xsl:value-of select="sl:note"/></dd>
	  </xsl:if>
	</dl>

	<xsl:for-each select="document(concat(@name,'.xml'),/)">
	  <table class="pretty">
	    <xsl:copy-of select="/*/*"/>
	  </table>
	</xsl:for-each>

    </body>
  </html>
</esp:page>
    </ex:document>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
