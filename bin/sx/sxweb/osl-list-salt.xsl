<xsl:transform
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:ofp="http://oracc.org/ns/ofp/1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:xh="http://www.w3.org/1999/xhtml">
  <xsl:key name="ofp-sign" match="ofp:list" use="@l"/>
  <xsl:param name="ofpx"/>
  <xsl:template match="xh:tr">
    <xsl:copy>
      <xsl:copy-of select="*[1]|*[2]|*[3]|*[4]"/>
      <xsl:variable name="l" select="xh:td[1]/text()"/>
      <xsl:variable name="c" select="xh:td[4]/@class"/>
      <xsl:variable name="u" select="xh:td[4]/text()"/>
      <xsl:for-each select="document($ofpx)">
	<xsl:variable name="lnode" select="key('ofp-sign', $l)"/>
	<td>
	  <xsl:choose>
	    <xsl:when test="count($lnode/ofp:salts)>0">
	      <xsl:for-each select="$lnode/ofp:salts/*">
		<xsl:value-of select="concat('.',translate(.,'0123456789','₀₁₂₃₄₅₆₇₈₉'), '&#xa0;')"
			      /><span class="{$c} salt{.}"
			      ><xsl:value-of select="$u"/></span>
	      </xsl:for-each>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:for-each select="$lnode/../ofp:salts/*">
		<xsl:value-of select="concat('.',translate(.,'0123456789','₀₁₂₃₄₅₆₇₈₉'), '&#xa0;')"
			      /><span class="list-salt {$c} salt{.}"
			      ><xsl:value-of select="$u"/></span>
		<xsl:text> </xsl:text>
	      </xsl:for-each>
	    </xsl:otherwise>
	  </xsl:choose>
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
