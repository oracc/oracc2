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

  <!--
      	<xsl:for-each select="nss:step|nss:alt">
	  <xsl:apply-templates select="."/>
	</xsl:for-each>
-->
  
  <xsl:template match="nss:sys">
    <xsl:variable name="max-inst">
      <xsl:for-each select="nss:step|nss:alt">
	<xsl:sort data-type="number" order="descending" select="count(.//nss:inst)"/>
	<xsl:if test="position() = 1">
	  <xsl:value-of select="@unit"/>
	</xsl:if>
      </xsl:for-each>
    </xsl:variable>
    <xsl:message><xsl:value-of select="@name"/> max-inst=<xsl:value-of select="$max-inst"/></xsl:message>
    <h1><xsl:value-of select="@name"/></h1>
    <table>
      <tr class="ns-steps">
	<xsl:for-each select="nss:step|nss:alt">
	  <xsl:if test="not(@type='frac') and not(nss:step[1]/@type='frac')">
	    <td class="ns-mult">
	      <xsl:if test="not(position()=1)">
		<xsl:choose>
		  <xsl:when test="@type='frac'">
		    <xsl:text> / </xsl:text>
		  </xsl:when>
		  <xsl:when test="nss:step/@type='frac'">
		    <xsl:text> / </xsl:text>
		  </xsl:when>
		  <xsl:otherwise>
		    <xsl:text> = </xsl:text>
		  </xsl:otherwise>
		</xsl:choose>
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
	  </xsl:if>
	</xsl:for-each>
      </tr>
      <tr class="ns-aevs">
	<xsl:for-each select="nss:step">
	  <xsl:if test="not(@type='frac') and not(nss:step[1]/@type='frac')">
	    <xsl:if test="not(position() = last())"><td/></xsl:if>
	    <td class="ns-aev"><xsl:value-of select="nss:aev/@n div nss:aev/@d"/></td>
	  </xsl:if>
	</xsl:for-each>
      </tr>
      <xsl:variable name="sys" select="."/>
      <xsl:for-each select="*[@unit=$max-inst]//nss:inst">
	<xsl:variable name="nth" select="position()"/>
	<tr class="x">
	  <xsl:for-each select="$sys/nss:step|$sys/nss:alt/nss:step">
	    <xsl:if test="not(position() = last())"><td/></xsl:if>
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
		<xsl:otherwise>
		  <span class="ns-ucun {$font} {nss:inst[$nth]/@feat}">
		    <xsl:value-of select="nss:inst[$nth]/@ucun"/>
		  </span>
		  <xsl:if test="@type='frac' and string-length(nss:inst[$nth]/@ucun)>0">
		    <span class="ns->frac">&#xa0;<xsl:value-of select="@unit"/></span>
		  </xsl:if>
		</xsl:otherwise>
	      </xsl:choose>
	    </td>
	  </xsl:for-each>
	</tr>
      </xsl:for-each>
    </table>
  </xsl:template>

  <xsl:template match="nss:step">
  </xsl:template>

  <!-- This implementation is for when all count(nss:alt/nss:step/nss:inst) == 1 -->
  <xsl:template match="nss:alt">
    <td class="ns-alt">
      <xsl:text>(</xsl:text><br/>
      <xsl:for-each select="nss:step">
	<xsl:if test="position() > 1"><xsl:text>|&#xa0;</xsl:text></xsl:if>
	<xsl:value-of select="@unit"/>
      </xsl:for-each>
      <xsl:text>)</xsl:text>
    </td>
  </xsl:template>
  
</xsl:transform>
