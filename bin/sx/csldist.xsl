<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns:ex="http://exslt.org/common"
    xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:esp="http://oracc.org/ns/esp/1.0"
    xmlns:g="http://oracc.org/ns/gdl/1.0"
    exclude-result-prefixes="sl dc xh esp"
    version="1.0">

  <xsl:key name="data" match="xh:table" use="@k"/>
  
  <xsl:template match="sl:sign|sl:form">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:copy-of select="*"/>
      <xsl:variable name="k" select="@key"/>
      <xsl:for-each select="document('csldist.xml',/)">
	<xsl:for-each select="key('data',$k)">
	  <sl:distribution>
	    <xsl:copy-of select="."/>
	  </sl:distribution>
	</xsl:for-each>
      </xsl:for-each>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="*|text()">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>
  
</xsl:transform>
