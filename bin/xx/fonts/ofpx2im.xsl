<xsl:transform
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:ofp="http://oracc.org/ns/ofp/1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:xh="http://www.w3.org/1999/xhtml">

  <xsl:key name="ofp-list" match="ofp:list" use="concat(@l,../../@oid)"/>

  <xsl:output method="text" encoding="UTF-8"/>
  
  <xsl:template match="ofp:list">
    <xsl:variable name="i" select="generate-id()"/>
    <xsl:for-each select="key('ofp-list',concat(@l,../../@oid))">
      <!--<xsl:message>position=<xsl:value-of select="position()"/></xsl:message>-->
      <xsl:if test="position()=1 and generate-id(.)=$i">
	<xsl:value-of select="concat(ancestor::ofp:sign/@oid, '&#x9;', @l, '&#x9;')"/>
	<xsl:choose>
	  <xsl:when test="ofp:salts">
	    <xsl:call-template name="feat">
	      <xsl:with-param name="nodes" select="ofp:salts/*"/>
	    </xsl:call-template>
	  </xsl:when>
	  <xsl:when test="ancestor::ofp:sign/ofp:salts">
	    <xsl:call-template name="feat">
	      <xsl:with-param name="nodes" select="ancestor::ofp:sign/ofp:salts/*"/>
	    </xsl:call-template>
	  </xsl:when>
	</xsl:choose>
	<xsl:text>&#xa;</xsl:text>
      </xsl:if>
    </xsl:for-each>
  </xsl:template>

  <xsl:template name="feat">
    <xsl:param name="nodes"/>
    <xsl:for-each select="$nodes">
      <xsl:if test="not(.='0')">
	<xsl:value-of select="concat('salt', .)"/>
	<xsl:if test="not(position()=last())"><xsl:text> </xsl:text></xsl:if>
      </xsl:if>
    </xsl:for-each>
  </xsl:template>
  
  <xsl:template match="text()"/>
  
</xsl:transform>
