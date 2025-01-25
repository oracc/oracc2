<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
	       xmlns:nss="http://oracc.org/ns/numsys/1.0"
	       xmlns="http://www.w3.org/1999/xhtml"
	       xmlns:xh="http://www.w3.org/1999/xhtml"	       
	       >

  <xsl:param name="font" select="'ofs-noto'"/>
  
  <xsl:template match="nss:systems">
    <html>
      <head>
	<meta charset="UTF-8"/>
	<link rel="stylesheet" type="text/css" media="screen,projection,print" href="/css/fonts.css"/>
	<title>Number Systems</title>
      </head>
      <body>
	<xsl:apply-templates/>
      </body>
    </html>
  </xsl:template>

  <xsl:template match="nss:sys">
    <xsl:variable name="max-inst">
      <xsl:for-each select="nss:step">
	<xsl:sort data-type="number" order="descending" select="count(nss:inst)"/>
	<xsl:if test="position() = 1">
	  <xsl:value-of select="@unit"/>
	</xsl:if>
      </xsl:for-each>
    </xsl:variable>
    <!--<xsl:message><xsl:value-of select="@name"
	/> max-inst=<xsl:value-of select="$max-inst"/></xsl:message>-->
    <h1><xsl:value-of select="@name"/></h1>
    <table>
      <tr class="ns-steps">
	<xsl:for-each select="nss:step">
	  <xsl:choose>
	    <xsl:when test="not(@type='frac')">
	      <td class="ns-mult">
		<xsl:if test="not(position()=1)">
		  <xsl:text> = </xsl:text>
		</xsl:if>
		<xsl:choose>
		  <xsl:when test="nss:mul/@n = '0'">
		    <xsl:text>1</xsl:text>
		  </xsl:when>
		  <xsl:otherwise>
		    <xsl:value-of select="nss:mul/@n div nss:mul/@d"/>
		  </xsl:otherwise>
		</xsl:choose>
		<xsl:text> × </xsl:text>
	      </td>
	      <td class="ns-step"><xsl:value-of select="@unit"/></td>
	    </xsl:when>
	    <xsl:otherwise>
	      <td>&#xa0;</td><!-- spacer instead of '= N ×' -->
	      <td>/</td>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:for-each>
      </tr>
      <tr class="ns-aevs">
	<xsl:for-each select="nss:step">
	  <xsl:choose>
	    <xsl:when test="not(@type='frac')">
	      <!--<xsl:if test="not(position() = last())"><td/></xsl:if>-->
	      <td/>
	      <td class="ns-aev"><xsl:value-of select="nss:aev/@n div nss:aev/@d"/></td>
	    </xsl:when>
	    <xsl:otherwise>
	      <td class="ns-aev"/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:for-each>
      </tr>
      <xsl:variable name="sys" select="."/>
      <xsl:for-each select="*[@unit=$max-inst]//nss:inst">
	<xsl:variable name="nth" select="position()"/>
	<tr class="x">
	  <xsl:for-each select="$sys/nss:step">
	    <!--<xsl:if test="not(position() = last())"><td/></xsl:if>-->
	    <td/>
	    <td>
	      <xsl:choose>
		<xsl:when test="@type='word'">
		  <xsl:if test="$nth=1">
		    <span class="{$font} {@feat}">
		      <xsl:value-of select="@axis-ucun"/>
		      <xsl:text> </xsl:text>
		      <xsl:value-of select="@ucun"/>
		    </span>
		  </xsl:if>
		</xsl:when>
		<xsl:when test="@unit='(gur)'">
		  <xsl:if test="$nth=1">
		    <span class="{$font} {@feat}">
		      <xsl:value-of select="@axis-ucun"/>
		    </span>
		  </xsl:if>
		</xsl:when>
		<xsl:otherwise>
		  <span class="ns-ucun {$font} {nss:inst[$nth]/@feat}">
		    <xsl:value-of select="nss:inst[$nth]/@ucun"/>
		  </span>
		  <xsl:if test="@type='frac' and string-length(nss:inst[$nth]/@ucun)>0">
		    <xsl:choose>
		      <xsl:when test="contains(nss:inst[$nth]/@n, 'igiŋal')">
			<span class="ns-frac">
			&#xa0;igi-<xsl:value-of select="nss:inst[$nth]/nss:count/@d"/>(diš)-ŋal₂</span>
		      </xsl:when>
		      <xsl:otherwise>
			<span class="ns-frac">&#xa0;<xsl:value-of select="nss:inst[$nth]/@n"/></span>
		      </xsl:otherwise>
		    </xsl:choose>
		  </xsl:if>
		</xsl:otherwise>
	      </xsl:choose>
	    </td>
	  </xsl:for-each>
	</tr>
      </xsl:for-each>
    </table>
  </xsl:template>
  
</xsl:transform>
