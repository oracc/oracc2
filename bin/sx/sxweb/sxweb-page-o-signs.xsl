<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:sxw="http://oracc.org/ns/sxweb/1.0"
	       xmlns:sl="http://oracc.org/ns/sl/1.0"
	       xmlns="http://www.w3.org/1999/xhtml"
	       xmlns:esp="http://oracc.org/ns/esp/1.0"
	       version="1.0"
	       exclude-result-prefixes="sl">

  <xsl:param name="project" select="'osl'"/>
  <xsl:include href="sxweb-util.xsl"/>

  <xsl:template match="sxw:page-o-signs">
    <xsl:variable name="codes" select="@codepoints"/>
    <xsl:variable name="stats" select="@stats"/>
    <table class="page-o-signs pretty">
      <xsl:choose>
	<xsl:when test="@stats='yes'">
	  <xsl:for-each select="*/*"> <!--no ns on oids/oid so use */* as workaround -->
	    <xsl:sort data-type="number" select="@freq" order="descending"/>
	    <xsl:sort data-type="number" select="@sort"/>
	    <xsl:variable name="freq" select="@freq"/>
	    <xsl:variable name="id" select="@xml:id"/>
	    <xsl:for-each select="document('/home/oracc/osl/02xml/sl.xml')">	      
	      <xsl:for-each select="id($id)">
		<xsl:call-template name="sxw-pos-tr">
		  <xsl:with-param name="codes" select="$codes"/>
		  <xsl:with-param name="freq" select="$freq"/>
		  <xsl:with-param name="stats" select="$stats"/>
		</xsl:call-template>
	      </xsl:for-each>
	    </xsl:for-each>
	  </xsl:for-each>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:variable name="ids" select="text()"/>
	  <xsl:for-each select="document('/home/oracc/osl/02xml/sl.xml')">
	    <xsl:for-each select="id($ids)">
	      <xsl:sort data-type="number" select="@sort"/>
	      <xsl:call-template name="sxw-pos-tr">
		<xsl:with-param name="codes" select="$codes"/>
		<xsl:with-param name="stats" select="$stats"/>
	      </xsl:call-template>
	    </xsl:for-each>
	  </xsl:for-each>
	</xsl:otherwise>
      </xsl:choose>
    </table>
  </xsl:template>

  <xsl:template name="sxw-pos-tr">
    <xsl:param name="codes"/>
    <xsl:param name="freq"/>
    <xsl:param name="stats"/>
    <tr>
      <xsl:if test="$stats='yes'">
	<xsl:call-template name="sxw-pos-freq-td">
	  <xsl:with-param name="freq" select="$freq"/>
	</xsl:call-template>
      </xsl:if>
      <xsl:if test="$codes='yes'">
	<xsl:call-template name="sxw-pos-codepoint-td"/>
      </xsl:if>
      <xsl:for-each select=".">
	<xsl:call-template name="sxw-pos-summary">
	  <xsl:with-param name="stats" select="$stats"/>
	</xsl:call-template>
      </xsl:for-each>
    </tr>
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
  
  <xsl:template name="sxw-pos-freq-td">
    <xsl:param name="freq"/>
    <td class="sxw-pos-freq">
      <xsl:value-of select="$freq"/>
    </td>
  </xsl:template>
  
  <xsl:template name="sxw-pos-link-td">
    <td class="sxw-pos-link">
      <xsl:choose>
	<xsl:when test="self::sl:form">
	  <esp:link url="{concat('/',$project,'/signlist/', ../@xml:id)}">
	    <span class="snames"><xsl:value-of select="@n"/></span>
	  </esp:link>
	</xsl:when>
	<xsl:otherwise>
	  <esp:link url="{concat('/',$project,'/signlist/', @xml:id)}">
	    <span class="snames"><xsl:value-of select="ancestor-or-self::sl:sign/@n"/></span>
	  </esp:link>
	</xsl:otherwise>
      </xsl:choose>
    </td>
  </xsl:template>

  <xsl:template name="sxw-pos-ucun-td">
    <td class="sxw-pos-ucun ofs-noto">
      <xsl:choose>
	<xsl:when test="sl:ucun">
	  <xsl:call-template name="sxweb-ucun"/>
	</xsl:when>
	<xsl:when test="@oid">
	  <xsl:for-each select="id(@oid)">
	    <xsl:call-template name="sxweb-ucun"/>
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
