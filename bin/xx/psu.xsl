<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
	       xmlns:xcl="http://oracc.org/ns/xcl/1.0"
	       xmlns:xtf="http://oracc.org/ns/xtf/1.0"
	       xmlns:xl="http://www.w3.org/1999/xlink"
	       >

  <xsl:template match="xcl:linkset[@xl:role='psu']">
    <xsl:variable name="ref" select="substring-after(xcl:link[1]/@xl:href, '#')"/>
    <xsl:variable name="lem" select="id($ref)"/>
    <xsl:variable name="l" select="id($lem/@ref)"/>
    <xsl:value-of select="$l/../@n"/>
    <xsl:text>.&#x9;</xsl:text>
    <xsl:value-of select="@xl:title"/>
    <xsl:text>&#xa;</xsl:text>
  </xsl:template>

  <xsl:template match="text()"/>

</xsl:transform>
