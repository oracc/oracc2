<xsl:transform
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:ofp="http://oracc.org/ns/ofp/1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:xh="http://www.w3.org/1999/xhtml">
  <xsl:key name="ofp-sign" match="ofp:sign" use="@list"/>
  <xsl:param name="ofpx"/>
  <xsl:template match="xh:tr">
    <xsl:copy>
      <xsl:copy-of select="*[1]|*[2]|*[3]|*[4]"/>
      <xsl:variable name="l" select="xh:td[1]/text()"/>
      <xsl:variable name="c" select="xh:td[4]/@class"/>
      <xsl:variable name="u" select="xh:td[4]/text()"/>
      <xsl:for-each select="document($ofpx)">
	<xsl:variable name="salts" select="key('ofp-sign', $l)/ofp:salts"/>
	<td>
	  <xsl:if test="count($salts)>0">
	    <xsl:for-each select="$salts/*">
	      <xsl:value-of select="concat('.',translate(.,'0123456789','₀₁₂₃₄₅₆₇₈₉'), '&#xa0;')"
			    /><span class="{$c} salt{.}"
			    ><xsl:value-of select="$u"/></span>
	      <xsl:if test="not(position()=last())"><xsl:text>, </xsl:text></xsl:if>
	    </xsl:for-each>
	    <xsl:text>.</xsl:text>
	  </xsl:if>
	</td>
      </xsl:for-each>
      <xsl:copy-of select="*[5]|*[6]"/>
    </xsl:copy>
  </xsl:template>
  <xsl:template match="*|text()">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>
</xsl:transform>
