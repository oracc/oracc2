<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns="http://www.w3.org/1999/xhtml"
		xmlns:esp="http://oracc.org/ns/esp/1.0"
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:o="http://oracc.org/ns/oracc/1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="lex:phra-base-data">
  <body>
    <xsl:apply-templates/>
  </body>
</xsl:template>

<xsl:template match="lex:group[@type='word']">
  <div class="lex-word" title="{@value}" o:id="{@oid}">
    <!-- <h3 class="lex-base">Pronunciation Data</h3> -->
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="lex:group[@type='base']">
  <div class="lex-base" title="{@value}">
    <!-- href="/{descendant::lex:data/@project}/{@lang}?xis={../@xis}" -->
    <h2 class="lex-base"><a href="javascript:distprof2('{/*/@project}','{@lang}','{../@xis}')"
			    ><xsl:value-of select="@value"/></a></h2>
    <xsl:for-each select="*"> <!-- spel groups -->
      <p class="lex-line">
	<xsl:call-template name="emit-line"/>
	<xsl:text> (</xsl:text>
	<xsl:for-each select="*"> <!-- ref groups -->
	  <xsl:apply-templates select="."/>
	  <xsl:if test="not(position()=last())">
	    <xsl:text>; </xsl:text>
	  </xsl:if>
	</xsl:for-each>
	<xsl:text>).</xsl:text>
      </p>
    </xsl:for-each>
  </div>
</xsl:template>

<xsl:template name="emit-line">
  <xsl:for-each select="lex:group[1]/lex:data[1]">
    <span class="lex-spel">
      <xsl:value-of select="@spel"/>
      <xsl:if test="string-length(@read) > 0">
	<xsl:text> = </xsl:text>
	<i><xsl:value-of select="@read"/></i>
      </xsl:if>
      <xsl:if test="string-length(lex:eq/@form) > 0">
	<xsl:text> = </xsl:text>
	<i><xsl:value-of select="lex:eq/@form"/></i>
      </xsl:if>
    </span>
  </xsl:for-each>
</xsl:template>

<xsl:template match="lex:group[@type='refs']">
  <xsl:variable name="word" select="lex:data/*/lex:word/@wref[1]"/>
  <xsl:if test="string-length($word)=0">
    <xsl:message><xsl:value-of select="concat(*[1]/@id_text,':',*[1]/@label)"/>: lex:data/@sref is empty</xsl:message>
  </xsl:if>
  <xsl:variable name="text">
    <xsl:choose>
      <xsl:when test="contains($word,'.')">
	<xsl:value-of select="substring-before($word,'.')"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="$word"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="line-dots">
    <xsl:choose>
      <xsl:when test="contains($word,'.')">
	<xsl:value-of select="substring-after($word,'.')"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="$word"/> <!-- shouldn't be able to happen -->
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="line">
    <xsl:choose>
      <xsl:when test="contains($line-dots,'.')">
	<xsl:value-of select="substring-before($line-dots,'.')"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="$line-dots"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="proj" select="lex:data[1]/@project"/>
  <a href="http://oracc.org/{$proj}/{$text}.{$line}" title="{$proj} on Oracc">
    <xsl:value-of select="lex:data[1]/@n"/>
    <xsl:text> </xsl:text>
    <xsl:for-each select="lex:data/@label">
      <xsl:value-of select="."/>
      <xsl:if test="not(position()=last())">
	<xsl:text>, </xsl:text>
      </xsl:if>
    </xsl:for-each>
  </a>
</xsl:template>

</xsl:stylesheet>
