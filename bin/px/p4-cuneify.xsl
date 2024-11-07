<xsl:stylesheet 
    xmlns:gdl="http://oracc.org/ns/gdl/1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:xtf="http://oracc.org/ns/xtf/1.0"
    version="1.0">
<xsl:output method="xml" encoding="utf-8" indent="yes"/>

<xsl:param name="period" select="'na'"/>

<xsl:param name="dfont" select="'ofs-noto'"/>
<xsl:param name="dmag" select="'ofs-100'"/>
<xsl:param name="dscript" select="'middle'"/>

<xsl:variable name="font">
  <xsl:choose>
    <xsl:when test="/*/@fnt"><xsl:value-of select="/*/@fnt"/></xsl:when>
    <xsl:otherwise><xsl:value-of select="$dfont"/></xsl:otherwise>
  </xsl:choose>
</xsl:variable>

<xsl:variable name="mag">
  <xsl:choose>
    <xsl:when test="/*/@mag"><xsl:value-of select="/*/@mag"/></xsl:when>
    <xsl:otherwise><xsl:value-of select="$dmag"/></xsl:otherwise>
  </xsl:choose>
</xsl:variable>

<xsl:variable name="script">
  <xsl:choose>
    <xsl:when test="/*/@script"><xsl:value-of select="/*/@scr"/></xsl:when>
    <xsl:otherwise><xsl:value-of select="$dscript"/></xsl:otherwise>
  </xsl:choose>
</xsl:variable>

<xsl:variable name="apos">
  <xsl:text>'</xsl:text>
</xsl:variable>

<xsl:template match="/">
  <xsl:message>p4cuneify.xsl using font=<xsl:value-of select="$font"/></xsl:message>
  <html>
    <head>
      <title>Cuneified <xsl:value-of select="/*/@n"/></title>
      <link rel="stylesheet" type="text/css" href="/css/p4-cuneify.css" />
      <link rel="stylesheet" type="text/css" href="/css/fonts.css" />
    </head>
    <body>
      <xsl:apply-templates select="xtf:transliteration|xtf:composite
				   |*/xtf:transliteration|*/xtf:composite"/>
    </body>
  </html>
</xsl:template>

<xsl:template match="xtf:transliteration|xtf:composite">
  <h1>Cuneified <xsl:value-of select="@n"/></h1>
  <p class="disclaimer">[This cuneiform text was computer-generated
  from a transliteration.<!--<a href="/cuneify-trouble.html">See this page for trouble-shooting tips.</a>-->]</p>
  <table class="cuneify-text">
    <xsl:apply-templates/>
  </table>
</xsl:template>

<xsl:template match="xtf:l">
  <tr class="cuneify-line cuneiform">
    <td class="cuneify-label">
      <xsl:variable name="num" select="number(translate(@n,$apos,''))"/>
      <xsl:choose>
	<xsl:when test="count(preceding-sibling::xtf:l) = 0
			or $num mod  5 = 0
			or $num mod 10 = 0">
			<xsl:value-of select="@label"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:text>&#xa0;</xsl:text>
	</xsl:otherwise>
      </xsl:choose>
    </td>
    <td>
      <p class="cuneify-content {$font} {$script} {$mag}">
	<xsl:apply-templates/>
      </p>
    </td>
  </tr>
</xsl:template>

<!-- drop sub-line variants -->
<xsl:template match="xtf:v"/>

<xsl:template match="gdl:w">
  <xsl:for-each select=".//*[@gdl:utf8 or @gdl:type='ellipsis']">
    <xsl:if test="contains(@gdl:o,'[')"><span class="osquare">&#x200D;</span></xsl:if>
    <xsl:choose>
      <xsl:when test="@gdl:break='missing'">
	<xsl:choose>
	  <xsl:when test="contains(@gdl:utf8,'&#x2591;')">
	    <xsl:value-of select="@gdl:utf8"/>
	  </xsl:when>
	  <xsl:otherwise>
	    <span class="broken">
	      <xsl:choose>
		<xsl:when test="@gdl:type='ellipsis'"> <!-- gdl:x -->
		  <span class="ellipsis"
			>.&#xa0;.&#xa0;.<xsl:if
			test="../following-sibling::*[1]/gdl:x[@gdl:type='ellipsis']">&#xa0;</xsl:if
			></span>
		</xsl:when>
		<xsl:when test="translate(@gdl:utf8,'X','x')='x'">
		  <span class="roman">&#xd7;</span>
		</xsl:when>
		<xsl:otherwise>
		  <span class="{ancestor-or-self::*[@fnt][1]/@fnt}">
		    <xsl:choose>
		      <xsl:when test="@gdl:salt">
			<span class="salt{@gdl:salt}"><xsl:value-of select="@gdl:utf8"/></span>
		      </xsl:when>
		      <xsl:otherwise>
			<xsl:value-of select="@gdl:utf8"/>
		      </xsl:otherwise>
		    </xsl:choose>
		  </span>
		</xsl:otherwise>
	      </xsl:choose>
	    </span>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:when>
      <xsl:otherwise>
	<span class="{ancestor-or-self::*[@fnt][1]/@fnt}">
	  <xsl:choose>
	    <xsl:when test="@gdl:salt">
	      <span class="salt{@gdl:salt}"><xsl:value-of select="@gdl:utf8"/></span>
	    </xsl:when>
	    <xsl:when test="@gdl:type='ellipsis'"> <!-- gdl:x -->
	      <span class="ellipsis">.&#xa0;.&#xa0;.<xsl:if
	      test="../following-sibling::*[1]/gdl:x[@gdl:type='ellipsis']">&#xa0;</xsl:if
	      ></span>
	    </xsl:when>
	    <xsl:when test="translate(@gdl:utf8,'X','x')='x'">
	      <span class="roman broken gray">&#xd7;</span>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:value-of select="@gdl:utf8"/>
	    </xsl:otherwise>
	  </xsl:choose>
	</span>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:if test="contains(@gdl:c,']')"><span class="csquare">&#x200D;</span></xsl:if>
  </xsl:for-each>
  <xsl:if test="not(@form='x') and not(@form='(xx)')
		and not('x'=substring(@form,string-length(@form)))
		and not('x)'=substring(@form,string-length(@form)-1))
		and not(position()=last())">
    <xsl:text> </xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="gdl:nonw">
  <xsl:for-each select=".//*[@gdl:utf8]">
    <span class="{ancestor-or-self::*[@fnt][1]/@fnt}">
      <xsl:value-of select="@gdl:utf8"/>
    </span>
  </xsl:for-each>
  <xsl:text> </xsl:text>
</xsl:template>

<xsl:template match="xtf:note"/>

<xsl:template match="*">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
