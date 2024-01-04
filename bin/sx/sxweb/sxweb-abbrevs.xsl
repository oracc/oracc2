<xsl:stylesheet
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:esp="http://oracc.org/ns/esp/1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    version="1.0">

<xsl:template match="sl:signlist">
  <esp:page xmlns:esp="http://oracc.org/ns/esp/1.0"
	    xmlns="http://www.w3.org/1999/xhtml"
	    xmlns:xh="http://www.w3.org/1999/xhtml">
    <esp:name>Abbreviations</esp:name>
    <esp:title>Abbreviations</esp:title>
    <html>
      <head/>
      <body>
	<dl>
	  <xsl:for-each select="sl:listdef|sl:sysdef">
	    <xsl:sort select="@name"/>
	    <xsl:apply-templates select="."/>
	  </xsl:for-each>
	</dl>
      </body>
    </html>
  </esp:page>
</xsl:template>

<xsl:template match="sl:listdef">
  <dt><xsl:value-of select="@name"/></dt>
  <dd><xsl:apply-templates select="sl:lit|sl:note"/></dd>
</xsl:template>

<xsl:template match="sl:sysdef">
  <dt><xsl:value-of select="@name"/></dt>
  <dd><xsl:apply-templates select="sl:info|sl:lit|sl:note"/></dd>
</xsl:template>

<xsl:template match="sl:info|sl:lit|sl:note">
  <p><xsl:value-of select="text()"/></p>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
