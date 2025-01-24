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
    <h1><xsl:value-of select="@name"/></h1>
    <table>
      <tr class="ns-steps">
	<xsl:for-each select="nss:step">
	  <xsl:if test="not(position() = last())">
	    <td class="ns-mult"> = <xsl:value-of select="nss:mul/@n div nss:mul/@d"/> × </td>
	  </xsl:if>
	  <td class="ns-step"><xsl:value-of select="@unit"/></td>
	</xsl:for-each>
      </tr>
      <tr class="ns-aevs">
	<xsl:for-each select="nss:step">
	  <xsl:if test="not(position() = last())"><td/></xsl:if>
	  <td class="ns-aev"><xsl:value-of select="nss:aev/@n div nss:aev/@d"/></td>
	</xsl:for-each>
      </tr>
      <xsl:variable name="sys" select="."/>
      <xsl:for-each select="nss:step[@unit=$max-inst]/nss:inst">
	<xsl:variable name="nth" select="position()"/>
	<tr class="x">
	  <xsl:for-each select="$sys/nss:step">
	    <xsl:if test="not(position() = last())"><td/></xsl:if>
	    <td><span class="{$font} {nss:inst[$nth]/@feat}"><xsl:value-of select="nss:inst[$nth]/@ucun"/></span></td>
	  </xsl:for-each>
	</tr>
      </xsl:for-each>
    </table>
  </xsl:template>
  
</xsl:transform>
