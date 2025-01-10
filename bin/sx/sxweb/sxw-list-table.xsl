<xsl:transform
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:ofp="http://oracc.org/ns/ofp/1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:xh="http://www.w3.org/1999/xhtml">

  <xsl:key name="ofp-sign" match="ofp:list" use="@l"/>

  <xsl:param name="font"/>
  <xsl:param name="ofpx"/>

  <xsl:template match="list">
    <xh:table class="pretty">
      <xsl:apply-templates/>
    </xh:table>
  </xsl:template>

  <xsl:template match="entry">
    <xh:tr>
      <xh:td><xsl:value-of select="name"/></xh:td>
      <xh:td><xsl:value-of select="sfname|lsname"/></xh:td>
      <xh:td>
	<xsl:if test="ucun|lucun">
	  <xsl:value-of select="ucun"/>
	</xsl:if>
      </xh:td>
      <xh:td>
	<xsl:choose>
	  <xsl:when test="limage">
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:value-of select="ucun|lucun"/>
	  </xsl:otherwise>
	</xsl:choose>
      </xh:td>
      <xsl:variable name="l" select="name"/>
      <xsl:variable name="c" select="{$font} ofs-200"/>
      <xsl:variable name="u" select="ucun"/>
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
      <xh:td><xsl:value-of select="ucode"/></xh:td>
      <xh:td><xsl:value-of select="note"/></xh:td>
    </xh:tr>
  </xsl:template>

  <!--
      <xsl:template match="*|text()">
      <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
      </xsl:copy>
      </xsl:template>
  -->
</xsl:transform>
