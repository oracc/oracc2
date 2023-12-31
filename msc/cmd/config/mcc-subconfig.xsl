<xsl:stylesheet 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:xpd="http://oracc.org/ns/xpd/1.0"
    xmlns="http://oracc.org/ns/xpd/1.0"
    version="1.0">

<xsl:output method="xml" indent="yes" encoding="utf-8"/>
<xsl:variable name="oracc" select="'/home/oracc/'"/>

<xsl:template match="xpd:project">
  <project>
    <xsl:copy-of select="@n"/>
    <xsl:comment>&#xa;DO NOT EDIT THIS FILE: EDIT subconfig.xml INSTEAD&#xa;</xsl:comment>
    <xsl:apply-templates mode="top"/>
  </project>
</xsl:template>

<xsl:template mode="top" 
	      match="xpd:name|xpd:abbrev|xpd:project-type|xpd:public|xpd:blurb|xpd:image|xpd:image-alt|xpd:type|xpd:logo">
  <xsl:copy-of select="."/>
</xsl:template>

<xsl:template mode="top" match="xpd:inherit">
  <xsl:apply-templates mode="inherit" 
		       select="document(concat($oracc,@project,'/00lib/config.xml'))"/>
</xsl:template>

<xsl:template mode="inherit" match="xpd:option|xpd:labels">
  <xsl:copy-of select="."/>
</xsl:template>

<xsl:template mode="inherit" match="text()"/>

</xsl:stylesheet>