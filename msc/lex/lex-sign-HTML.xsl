<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns="http://www.w3.org/1999/xhtml"
		xmlns:esp="http://oracc.org/ns/esp/1.0"
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:include href="lex-act-isid.xsl"/>

<xsl:template match="lex:sign-data">
  <body>
    <xsl:apply-templates/>
  </body>
</xsl:template>

<xsl:template match="lex:group[@type='sign']">
  <div class="lex-sign" title="{@value}" oid="{@oid}">
    <h2>
      <a href="javascript://" onclick="act_isid(event)">
	<xsl:call-template name="lex-act-isid">	<!-- '{*[1]/@lang}','sl.{@oid}.s' -->
	  <xsl:with-param name="isid" select="concat('sl.',@oid,'.s')"/>
	</xsl:call-template>
	<xsl:text>Lexical data for sign </xsl:text>
	<xsl:value-of select="@value"/>
      </a>
    </h2>
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="lex:group[@type='read']">
  <div class="lex-read" title="{@value}">
    <xsl:choose>
      <xsl:when test="string-length(@value)>0">
	<h2><xsl:value-of select="@value"/></h2>
      </xsl:when>
      <xsl:otherwise>
	<h2>(unspecified value)</h2>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="lex:group[@type='spel']">
  <p class="lex-spel" title="{@value}">
    <!--This should probably be revised to use gdl for the spellings,
         and perhaps for readings as well-->
    <span class="lex-spel-tlit"><xsl:value-of select="@value"/></span>
    <xsl:text>: </xsl:text>
    <!-- The input here has a refs wrapper with an empty xis
         attribute; should be able to fix this to link to spellings
         -->
    <xsl:for-each select="lex:group[@type='refs']">
      <esp:link url="http://oracc.org/{@project}/{*[1]/@sref}" site-name="{@project} on Oracc">
	<xsl:value-of select="@n"/>
	<xsl:text> </xsl:text>
	<xsl:value-of select="@clabel"/>
      </esp:link>
      <xsl:if test="not(position()=last())">
	<xsl:text>; </xsl:text>
      </xsl:if>
    </xsl:for-each>
    <xsl:text>.</xsl:text>
  </p>
</xsl:template>

</xsl:stylesheet>
