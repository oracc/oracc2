<xsl:transform
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:ofp="http://oracc.org/ns/ofp/1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:xh="http://www.w3.org/1999/xhtml">

  <xsl:param name="label"/>
  
  <xsl:output method="text" encoding="UTF-8"/>

  <xsl:template match="ofp:sign">
    <xsl:value-of select="concat(@oid, '&#xa;')"/>
  </xsl:template>
  
  <xsl:template match="text()"/>
  
</xsl:transform>
