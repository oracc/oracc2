<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns="http://www.w3.org/1999/xhtml"
		xmlns:esp="http://oracc.org/ns/esp/1.0"
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:g="http://oracc.org/ns/gdl/1.0"
		xmlns:o="http://oracc.org/ns/oracc/1.0"
		xmlns:note="http://oracc.org/ns/note/1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:param name="project" select="'dcclt'"/>
<xsl:include href="g2-gdl-HTML.xsl"/>
<xsl:include href="lex-act-isid.xsl"/>
<xsl:include href="lex-group@refs.xsl"/>

<xsl:template match="lex:word-phra-data">
  <body>
    <xsl:apply-templates/>
  </body>
</xsl:template>

<xsl:template match="lex:group[@type='word']">
  <xsl:if test="count(.//lex:group[@type='equi'][not(@value='#none')])>0">
    <div class="lex-word" title="{@value}" o:id="{@oid}" id="lexphrases">
      <h3 class="lex refs"><span class="lex-button">Lexical Associations</span></h3>
      <xsl:apply-templates select="*/lex:group[@type='equi']"/>
    </div>
  </xsl:if>
</xsl:template>

<xsl:template match="lex:group[@type='equi']">
  <xsl:if test="not(@value='#none')">
    <div class="lex-phra" title="{@value}">
      <h2 class="lex-phra">
	<a href="javascript://" onclick="act_isid(event)"> <!-- '{@lang}','{@xis}' -->
	  <xsl:call-template name="lex-act-isid">
	    <xsl:with-param name="caller" select="'lex-word-equi/group=equi'"/>
	    <xsl:with-param name="isid" select="@xis"/>
	  </xsl:call-template>
	  <xsl:value-of select="concat(@value, ' (// ', ../@value, ')')"/>
	</a>
      </h2>
      <xsl:for-each select="*"> <!-- line groups -->
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
  </xsl:if>
</xsl:template>

<xsl:template name="emit-line">
  <xsl:for-each select="*[1]/*[1]">
    <span class="lex-line">
      <xsl:for-each select="*[1]//lex:word">
	<xsl:apply-templates/>
	<xsl:if test="not(position()=last())"><xsl:text> </xsl:text></xsl:if>
      </xsl:for-each>
      <xsl:if test="string-length(lex:eq/@form) > 0">
	<xsl:text> = </xsl:text>
	<xsl:for-each select="lex:eq//lex:word">
	  <xsl:apply-templates/>
	  <xsl:if test="not(position()=last())"><xsl:text> </xsl:text></xsl:if>
	</xsl:for-each>
      </xsl:if>
    </span>
  </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
