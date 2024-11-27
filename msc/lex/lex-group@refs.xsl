<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns="http://www.w3.org/1999/xhtml"
		xmlns:esp="http://oracc.org/ns/esp/1.0"
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:g="http://oracc.org/ns/gdl/1.0"
		xmlns:o="http://oracc.org/ns/oracc/1.0"
		xmlns:note="http://oracc.org/ns/note/1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="lex:group[@type='refs']">
  <xsl:variable name="word" select="lex:data/*/lex:word/@wref[1]"/>
  <xsl:if test="string-length($word)=0">
    <xsl:message><xsl:value-of select="concat(*[1]/@id_text,':',*[1]/@label)"
    			       />: lex:data/lex:word/@wref[1] is empty</xsl:message>
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
  <xsl:variable name="proj" select="@project"/>
  <a href="javascript://" onclick="act_ilnk(event)" title="{$proj} on Oracc"
     data-proj="{$proj}" data-iref="{$text}.{$line}">
    <xsl:value-of select="@n"/>
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
