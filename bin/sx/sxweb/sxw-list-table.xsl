<xsl:transform
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:ofp="http://oracc.org/ns/ofp/1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:xh="http://www.w3.org/1999/xhtml">

  <xsl:key name="ofp-sign" match="ofp:list" use="@l"/>

  <xsl:param name="font"/>
  <xsl:param name="ofpx"/>

  <xsl:template match="list">
    <table class="pretty list-page">
      <xsl:apply-templates/>
    </table>
  </xsl:template>

  <xsl:template match="entry">
    <tr>
      <td><xsl:value-of select="name"/></td>
      <td>
	<xsl:choose>
	  <xsl:when test="string-length(lvalues)>0 and not(lsname='O')">
	    <xsl:value-of select="lsname"/>
	    <xsl:value-of select="concat(' (',lvalues,')')"/>
	  </xsl:when>
	  <xsl:when test="string-length(lvalues)>0">
	    <xsl:value-of select="lvalues"/>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:value-of select="sfname"/>
	  </xsl:otherwise>
	</xsl:choose>
      </td>
      <td><xsl:value-of select="ucun"/></td>
      <td>
	<xsl:choose>
	  <xsl:when test="string-length(lucun)>0">
	    <span class="{$font} ofs-200"><xsl:value-of select="lucun"/></span>
	  </xsl:when>
	  <xsl:when test="string-length(limage)>0">
	    <xsl:value-of select="limage"/>
	  </xsl:when>
	  <xsl:otherwise>
	     <span class="{$font} ofs-200"><xsl:value-of select="ucun"/></span>
	  </xsl:otherwise>
	</xsl:choose>
      </td>
      <xsl:variable name="l" select="name"/>
      <xsl:variable name="c" select="concat($font, ' ofs-200')"/>
      <xsl:variable name="u" select="ucun"/>
      <xsl:for-each select="document($ofpx)">
	<xsl:variable name="lnode" select="key('ofp-sign', $l)"/>
	<td>
	  <xsl:choose>
	    <xsl:when test="count($lnode/ofp:salts)>0">
	      <xsl:for-each select="$lnode/ofp:salts/*">
		<xsl:value-of select="concat('.',translate(.,'0123456789','₀₁₂₃₄₅₆₇₈₉'), '&#xa0;')"
			      /><span class="list-salt {$c} salt{.}"
			      ><xsl:value-of select="$u"/></span>
	      </xsl:for-each>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:for-each select="$lnode/ancestor::ofp:sign/ofp:salts/*">
		<xsl:value-of select="concat('.',translate(.,'0123456789','₀₁₂₃₄₅₆₇₈₉'), '&#xa0;')"
			      /><span class="list-salt {$c} salt{.}"
			      ><xsl:value-of select="$u"/></span>
		<xsl:text> </xsl:text>
	      </xsl:for-each>
	    </xsl:otherwise>
	  </xsl:choose>
	</td>
      </xsl:for-each>
      <td><xsl:value-of select="ucode"/></td>
      <td><xsl:value-of select="note"/></td>
    </tr>
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
