<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:sxw="http://oracc.org/ns/sxweb/1.0"
	       xmlns:sl="http://oracc.org/ns/sl/1.0"
	       xmlns="http://www.w3.org/1999/xhtml"
	       version="1.0"
	       exclude-result-prefixes="sl">
  
  <xsl:template match="sxw:page-o-signs">
    <xsl:variable name="codes" select="@codepoints"/>
    <xsl:variable name="ids" select="text()"/>
    <xsl:for-each select="document('/home/oracc/osl/02xml/sl.xml')">
      <table class="page-o-signs">
	<xsl:for-each select="id($ids)">
	  <xsl:sort data-type="number" select="@sort"/>
	  <tr>
	    <xsl:if test="$codes='yes'">
	      <xsl:call-template name="sxw-pos-codepoint-td"/>
	    </xsl:if>
	    <xsl:for-each select=".">
	      <xsl:call-template name="sxw-pos-summary">
		<xsl:with-param name="codes" select="$codes"/>
	      </xsl:call-template>
	    </xsl:for-each>
	  </tr>
	</xsl:for-each>
      </table>
    </xsl:for-each>
  </xsl:template>

  <!-- the context node must be sl:sign or sl:form here -->
  <xsl:template name="sxw-pos-summary">
    <xsl:call-template name="sxw-pos-link-td"/>
    <xsl:call-template name="sxw-pos-ucun-td"/>
    <td class="sxw-pos-vals-td">
      <xsl:choose>
	<xsl:when test="ancestor-or-self::sl:form">
	  <xsl:for-each select="ancestor-or-self::sl:form/sl:v[not(@deprecated='1')]">
	    <xsl:value-of select="@n"/>
	    <xsl:if test="not(position() = last())"><xsl:text> </xsl:text></xsl:if>
	  </xsl:for-each>	
	</xsl:when>
	<xsl:otherwise>
	  <xsl:for-each select="ancestor-or-self::sl:sign/sl:v[not(@deprecated='1')]">
	    <xsl:value-of select="@n"/>
	    <xsl:if test="not(position() = last())"><xsl:text> </xsl:text></xsl:if>
	  </xsl:for-each>	
	</xsl:otherwise>
      </xsl:choose>
    </td>
  </xsl:template>

  <xsl:template name="sxw-pos-codepoint-td">
    <td class="sxw-pos-code">
      <xsl:if test="starts-with(sl:ucun/@hex, 'U+')">
	<xsl:value-of select="substring-after(sl:ucun/@hex,'U+')"/>
      </xsl:if>
    </td>
  </xsl:template>
  
  <xsl:template name="sxw-pos-link-td">
    <td class="sxw-pos-link">
      <xsl:choose>
	<xsl:when test="self::sl:form">
	  <a href="{concat('/',/*/@project,'/signlist/', ../@xml:id)}">
	    <span class="snames"><xsl:value-of select="@n"/></span>
	  </a>
	</xsl:when>
	<xsl:otherwise>
	  <a href="{concat('/',/*/@project,'/signlist/', @xml:id)}">
	    <span class="snames"><xsl:value-of select="ancestor-or-self::sl:sign/@n"/></span>
	  </a>
	</xsl:otherwise>
      </xsl:choose>
    </td>
  </xsl:template>

  <xsl:template name="sxw-pos-ucun-td">
    <td class="sxw-pos-ucun noto">
      <xsl:choose>
	<xsl:when test="sl:ucun">
	  <xsl:value-of select="sl:ucun"/>
	</xsl:when>
	<xsl:when test="@oid">
	  <xsl:for-each select="id(@oid)">
	    <xsl:value-of select="sl:ucun"/>
	  </xsl:for-each>
	</xsl:when>
	<xsl:otherwise/><!-- nothing to display for Cuneiform -->
      </xsl:choose>
    </td>
  </xsl:template>

  <xsl:template match="*">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

</xsl:transform>
