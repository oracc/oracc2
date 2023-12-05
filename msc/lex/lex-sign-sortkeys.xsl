<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns:lex="http://oracc.org/ns/lex/1.0"
 		xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text" encoding="UTF-8"/>
  
<xsl:template match="lex:group[@type='read' or @type='spel']">
  <xsl:value-of select="@value"/><xsl:text>
</xsl:text>
  <xsl:apply-templates/>
</xsl:template>

</xsl:stylesheet>
