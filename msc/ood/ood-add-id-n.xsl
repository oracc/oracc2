<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
	       xmlns:o="http://oracc.org/ns/ood/1.0">
  <xsl:variable name="nrecord" select="count(/*/o:record)"/>  
  <xsl:template match="o:record">
    <xsl:variable name="id" select="count(preceding-sibling::*)+1"/>
    <xsl:variable name="padded-id">
      <xsl:choose>
	<xsl:when test="$nrecord > 9999">
	  <xsl:value-of select="format-number($id, '00000')"/>
	</xsl:when>
	<xsl:when test="$nrecord > 999">
	  <xsl:value-of select="format-number($id, '0000')"/>
	</xsl:when>
	<xsl:when test="$nrecord > 99">
	  <xsl:value-of select="format-number($id, '000')"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:value-of select="format-number($id, '00')"/>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:copy>
      <xsl:attribute name="xml:id">
	<xsl:value-of select="concat('o',$padded-id)"/>
      </xsl:attribute>
      <o:id><xsl:value-of select="$padded-id"/></o:id>
      <o:n><xsl:value-of select="z_calc_description"/></o:n>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>
  <xsl:template match="*|text()">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>
</xsl:transform>
